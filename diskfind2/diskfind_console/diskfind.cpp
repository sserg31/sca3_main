#include "diskfind.h"

#define BLOCKSIZE 512
#define TAILLEN 0

#include <QSharedMemory>
#include <QMutexLocker>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QThread>

Diskfind* Diskfind::m_instance = NULL;
QString Diskfind::m_configPath = "/var/config/scanner.conf";
QString Diskfind::m_tmpStoreName = "EXTSTORE";
QString Diskfind::m_defaultTmpStorePath = "/tmp";

extern bool print0;
extern bool analyseFS;
extern bool quiet;

Diskfind* Diskfind::instance()
{
    if (!m_instance)
        m_instance = new Diskfind;

    return m_instance;
}

Diskfind::Diskfind()
{
    m_fsParser = NULL;
    m_termFlag = false;
    m_termMutex = new QMutex;
    m_reportStarted = false;

    codecs["cp1251"] = QTextCodec::codecForName("CP1251");
    codecs["koi8r"] = QTextCodec::codecForName("KOI8-R");
    codecs["utf8"] = QTextCodec::codecForName("UTF-8");
    codecs["cp866"] = QTextCodec::codecForName("CP866");

    parseConfig();

    m_plugins = new PluginsLoader("plugins");
    m_plugins->load();

    if (analyseFS)
        m_plugins->loadFS();
}

Diskfind::~Diskfind()
{
    codecs.clear();

    delete m_termMutex;
    delete m_plugins;
}

void Diskfind::print_results(QVector<SearchResult*>* v)
{
    foreach (SearchResult *res, *v)
    {
        if (m_formats.testFlag(Stdout))
        {
            cout    << res->str.toUtf8().constData() << m_stdoutSep
                    << res->block << m_stdoutSep
                    << res->offset << m_stdoutSep
                    << res->enc.toUtf8().constData() << m_stdoutSep
                    << res->resulttype.toUtf8().constData();

            if (analyseFS)
                cout << m_stdoutSep << res->filepath.toUtf8().constData();

            if (m_stdoutSep != '\0')
                cout << endl;

            else
                cout << m_stdoutSep << m_stdoutSep << flush;
        }

        if (m_formats.testFlag(Xml))
        {
            m_xmlWriter.writeStartElement("text_issue");

            m_xmlWriter.writeTextElement("text",        res->str);
            m_xmlWriter.writeTextElement("sector",      QString("%1").arg(res->block));
            m_xmlWriter.writeTextElement("offset",      QString("%1").arg(res->offset));
            m_xmlWriter.writeTextElement("encoding",    res->enc);
            m_xmlWriter.writeTextElement("type",        res->resulttype);
            m_xmlWriter.writeTextElement("filepath",    res->filepath);

            m_xmlWriter.writeEndElement();
        }
    }
}


void Diskfind::scanDevice(const QString &dev, const QStringList &dict, const QStringList &enc, const QStringList &types, const QString &cmdline, const QString &progressIPC)
{
    m_dev = dev;
    m_dict = dict;
    m_enc = enc;
    m_types = types;
    m_cmdLine = cmdline;

    if (!print0)
        m_stdoutSep = ':';
    else
        m_stdoutSep = '\0';

    if (analyseFS)
    {
        detectFSType(dev);

        if (m_fsParser)
        {
            if (!m_fsParser->prepare(m_tmpStorePath, dev))
            {
                cerr << "fail to init filesystem parser" << endl;
                m_fsParser = NULL;
            }
        }
        else
            cerr << "no fs parser detected" << endl;
    }

    beginReports();

    QByteArray buf;
    QFile f(dev);
    qint64 devSize = 0;

    if (!f.open(QIODevice::ReadOnly))
    {
        cerr << "Couldn't open device" << endl;
        endReports();
        qApp->exit(-1);
    }

    QFile part("/proc/partitions");
    if (!part.open(QIODevice::ReadOnly))
    {
        cerr << "Couldn't open partition table" << endl;
        endReports();
        qApp->exit(-1);
    }

    QString line = part.readLine();
    while (!line.isEmpty())
    {
        QStringList slist = line.simplified().split(QRegExp("[ \t]"));

        if (slist.count() < 3)
        {
            line = part.readLine();
            continue;
        }

        if (dev.endsWith(slist[3]))
        {
            devSize = slist[2].toLongLong();
        }

        line = part.readLine();
    }

    part.close();

    bool typed_scan = true;
    QVector<FileSigs>* sigs = m_plugins->get_sigs(types);
    if (!sigs)
        typed_scan = false;

    QVector< SearchResult*> *disk_tmp=0;
    QVector< SearchResult*> *typed_tmp=0;
    MultiDict* multidict = create_multi_coding_dict(dict,enc);

    buf.append(f.read(BLOCKSIZE));
    long long unsigned block = -1;

    QSharedMemory sharedMem(progressIPC);
    float curPos = 0, prevPos = 0;
    qint64 bytesRead = 0;

    if (!progressIPC.isEmpty())
    {
        if (!quiet)
            cerr << "attaching " << progressIPC.toUtf8().constData() << endl;

        sharedMem.attach(QSharedMemory::ReadWrite);
    }

    while (buf.size() && !getTermFlag())
    {
        block++;
        disk_tmp = scan_dict_buf(&buf, *multidict, block, "RAW" ,0);
        if (disk_tmp)
        {
            print_results(disk_tmp);
            erase_pointer_vector(disk_tmp);
            disk_tmp = 0;
        }

        if (typed_scan)
        {
            typed_tmp = scan_typed_files(ParserCreator(dev,buf,block,BLOCKSIZE-TAILLEN, sigs).search(),*multidict);
            if (typed_tmp)
            {
                print_results(typed_tmp);
                erase_pointer_vector(typed_tmp);
                typed_tmp = 0;
            }
        }

        bytesRead += BLOCKSIZE - TAILLEN;
        buf.remove(0, BLOCKSIZE - TAILLEN);
        buf.append(f.read(BLOCKSIZE-TAILLEN));

        curPos = (((float)bytesRead) / 1024 / devSize) * 100;

        if ((curPos - prevPos > 1) && sharedMem.isAttached()) //avoiding wasting time
        {
            prevPos = curPos;
            sharedMem.lock();
            memcpy(sharedMem.data(), &curPos, sizeof(curPos));
            sharedMem.unlock();
        }        
    }

    if(sharedMem.isAttached())
    {
        sharedMem.lock();
        memcpy(sharedMem.data(), &curPos, sizeof(curPos));
        sharedMem.unlock();
    }

    endReports();

    if (analyseFS && m_fsParser)
        m_fsParser->finalize();

    delete sigs;

    qDeleteAll(multidict->begin(), multidict->end());
    delete multidict;

    qApp->quit();
}


QVector<SearchResult*>* Diskfind::scan_typed_files(QVector<TypedFile*>* files, MultiDict dict)
{
    if (!files)
        return 0;

    QVector< SearchResult*> *typed_tmp=0;
    QVector<SearchResult*>* result = new QVector<SearchResult*>;

    foreach (TypedFile *file, *files)
    {
        typed_tmp = scan_dict_buf(file->buf, dict, file->pos->block, file->pos->type, file->pos->offset);

        if (concat_vectors(result, typed_tmp))
        {
            delete typed_tmp;
            typed_tmp = 0;
        }
    }

    if (result->isEmpty())
    {
        erase_pointer_vector(result);
        result = 0;
    }

    return result;
}

QVector<SearchResult*>* Diskfind::scan_dict_buf(QByteArray* buf,
                                                MultiDict dict,
                                                long long unsigned block,
                                                const QString& type,
                                                long long unsigned offset )
{
    QVector<SearchResult*>* lst = new QVector<SearchResult*>;
    // For encoding
    MultiDict::iterator j;
    for (j = dict.begin(); j != dict.end(); ++j)
    {
        QList<QByteArray>* encdict = j.value();
        // For words in encoding
        foreach (const QByteArray &d, *encdict)
        {
            int from = 0;
            while(from != -1)
            {
                if ((from = scan_word_buffer(buf, d, from)) != -1)
                {
                    SearchResult* tmp = new SearchResult;
                    if (!offset){
                        tmp->block = block + from / (BLOCKSIZE-TAILLEN);
                        tmp->offset = from % (BLOCKSIZE-TAILLEN) ;
                    }
                    else {
                        tmp->block = block + offset / (BLOCKSIZE-TAILLEN);
                        tmp->offset = offset % (BLOCKSIZE - TAILLEN);
                    }

                    if (analyseFS && m_fsParser)
                        tmp->filepath = m_fsParser->getFilename(block * BLOCKSIZE + offset);

                    tmp->str = codecs[j.key()]->toUnicode(d);
                    tmp->enc = j.key();
                    tmp->resulttype = type;
                    from += d.size();
                    lst->push_back(tmp);
                }
            }
        }
    }
    if (lst->isEmpty())
    {
        erase_pointer_vector(lst);
        lst=0;
    }
    return lst;
}



int Diskfind::scan_word_buffer(QByteArray* buf,
                               const QByteArray& word, int from)
{
    return buf->indexOf(word,from);
}

MultiDict* Diskfind::create_multi_coding_dict(const QStringList& dict,
                                              const QStringList& enc)
{
    QMap<QString,QList<QByteArray>*>* result = new QMap<QString,QList<QByteArray>*>;
    QVector<bool> in_d(dict.size());
    int i;

    foreach (const QString &e, enc)
    {
        QList<QByteArray>* multidict = new QList<QByteArray>;
        (*result)[e] = multidict;
        i = 0;
        foreach (const QString &word, dict)
        {
            // Latin word and have been added
            bool latin = only_latin(word);
            if (latin && in_d[i])
                continue;

            multidict->push_back(codecs[e]->fromUnicode(word));
            if (latin)
                in_d[i]=true;
            i++;
        }
    }
    return result;
}

bool Diskfind::only_latin(const QString& word)
{
    foreach(const QChar &ch, word)
        if (!isalnum(ch.toAscii()))
            return false;

    return true;
}


int Diskfind::concat_vectors(QVector<SearchResult*>* res, QVector<SearchResult*>* op)
{
    if (op)
    {
        foreach (SearchResult* r, *op)
            res->push_back(r);
        return 1;
    }
    return 0;

}

int Diskfind::erase_pointer_vector(QVector<SearchResult*>* vec)
{
    if (!vec)
        return 0;

    qDeleteAll(*vec);
    delete vec;

    return 1;
}

void Diskfind::setOutputFiles(const OutputFiles &files)
{
    foreach (OutputFormat format, files.keys())
        m_formats |= format;

    m_outputFiles = files;
}

void Diskfind::beginReports()
{
    m_beginScanTime = QDateTime::currentDateTime();

    if (m_formats.testFlag(Xml) && !m_outputFiles.value(Xml).isEmpty())
    {
        m_xmlFile.setFileName(m_outputFiles.value(Xml));
        m_xmlFile.open(QIODevice::WriteOnly);

        m_xmlWriter.setDevice(&m_xmlFile);
        m_xmlWriter.setAutoFormatting(true);

        m_xmlWriter.writeStartDocument();

        m_xmlWriter.writeStartElement("diskfindrun");
        m_xmlWriter.writeAttribute("profile", m_cmdLine);
        m_xmlWriter.writeAttribute("scan_name", "");

        m_xmlWriter.writeStartElement("scaninfo");
        m_xmlWriter.writeStartElement("drives");

        m_xmlWriter.writeStartElement("drive");
        m_xmlWriter.writeAttribute("id", QString("%1").arg(0));
        m_xmlWriter.writeAttribute("file", m_dev);
        m_xmlWriter.writeEndElement();
        m_xmlWriter.writeEndElement();//drives

        m_xmlWriter.writeStartElement("filetypes");
        foreach (const QString &type, m_types)
            m_xmlWriter.writeTextElement("filetype", type);
        m_xmlWriter.writeEndElement();

        m_xmlWriter.writeStartElement("buzztexts");
        foreach (const QString &word, m_dict)
            m_xmlWriter.writeTextElement("buzztext", word);
        m_xmlWriter.writeEndElement();
        m_xmlWriter.writeEndElement();//scaninfo

        m_xmlWriter.writeStartElement("results");
        m_xmlWriter.writeStartElement("text_issues");
        m_xmlWriter.writeAttribute("drive_id", QString("%1").arg(0));
    }
    m_reportStarted = true;
}

void Diskfind::endReports()
{
    m_endScanTime = QDateTime::currentDateTime();

    if (m_formats.testFlag(Xml))
    {
        if (m_xmlFile.isOpen())
        {
            m_xmlWriter.writeEndElement();//text_issues
            m_xmlWriter.writeEndElement();//results

            m_xmlWriter.writeStartElement("runstats");
            m_xmlWriter.writeTextElement("created", QDateTime::currentDateTime().toString(Qt::ISODate));
            m_xmlWriter.writeTextElement("started", m_beginScanTime.toString(Qt::ISODate));
            m_xmlWriter.writeTextElement("finished", m_endScanTime.toString(Qt::ISODate));
            m_xmlWriter.writeEndElement();

            m_xmlWriter.writeEndElement();
            m_xmlWriter.writeEndDocument();
            m_xmlFile.close();
        }
    }
}

void Diskfind::setTermFlag(bool state)
{
    QMutexLocker locker(m_termMutex);

    m_termFlag = state;
}

bool Diskfind::getTermFlag()
{
    QMutexLocker locker(m_termMutex);

    return m_termFlag;
}

void Diskfind::detectFSType(const QString &dev)
{
    QString fstype;
    QProcess process;
    process.start("disktype " + dev);
    process.waitForFinished();

    QStringList lines = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
    int pos;

    foreach(const QString &str, lines)
    {
        pos = str.indexOf("file system");
        if (pos != -1)
        {
            fstype = str.left(pos).trimmed().toLower();
            break;
        }
    }

    // disktype don't know ext4, we check it
    if (fstype == "ext3")
    {
        process.start("file -s " + dev);
        process.waitForFinished();

        if (process.readAllStandardOutput().contains("ext4"))
            fstype = "ext4";
    }

    if (!fstype.isEmpty())
        m_fsParser = m_plugins->getFSParser(fstype);
}

void Diskfind::parseConfig()
{
    QSettings setting(m_configPath, QSettings::IniFormat);

    switch(setting.status())
    {
        case QSettings::AccessError: cerr << "unable to open config file " << m_configPath.toUtf8().constData() << endl;
                    break;

        case QSettings::FormatError: cerr << "unable to parse config file " << m_configPath.toUtf8().constData() << endl;
                    break;

        default:
                    break;
    }

    QVariant value = setting.value(m_tmpStoreName);

    if (value.isNull())
        m_tmpStorePath = m_defaultTmpStorePath;

    else
        m_tmpStorePath = value.toString();
}

void Diskfind::timerEvent(QTimerEvent *)
{
    if (getTermFlag())
    {
        if (m_reportStarted)
            endReports();

        qApp->quit();
    }
}
