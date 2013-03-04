#include <QFile>
#include <QSet>
#include <QTemporaryFile>

#include "docx.h"

//PK..
static const char m_trailer[] = { 0x50, 0x4B, 0x05, 0x06 };

const QList<QByteArray> DocxPlugin::trailers = QList<QByteArray>() << QByteArray(m_trailer, sizeof(m_trailer));

DocxPlugin::DocxPlugin()
{
    FILESIZE = 5*1024*1024;
    MAXREAD = 16*1024;
}

int DocxPlugin::OOXMLRootPos(const QByteArray &data) const
{
    const QByteArray rootHeader = "[Content_Types].xml";

    return data.indexOf(rootHeader);
}

bool DocxPlugin::isDocx(const QByteArray &data) const
{
    int rootPos = OOXMLRootPos(data);
    if (rootPos != -1 && rootPos < 0x20)
    {
        const QByteArray docxMainPart = "word/_rels/document.xml";
        int mainPartPos = data.indexOf(docxMainPart);

        return mainPartPos != -1 && mainPartPos > rootPos;
    }

    return false;
}

bool DocxPlugin::isXlsx(const QByteArray &data) const
{
    int rootPos = OOXMLRootPos(data);
    if (rootPos != -1 && rootPos < 0x20)
    {
        const QByteArray docxMainPart = "xl/_rels/workbook.xml";
        int mainPartPos = data.indexOf(docxMainPart);

        return mainPartPos != -1 && mainPartPos > rootPos;
    }

    return false;
}

QByteArray DocxPlugin::get_signature()
{
    static const char s[] = {0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00};
    return QByteArray(s, sizeof(s));
}

QString DocxPlugin::get_mnemonics()
{
    return "DOCX";
}

int DocxPlugin::set_params(const TypedFindPosition& pos)
{
    m_pos = pos;
    TMPFILENAME = QString("/tmp/sign.%1.%2").arg(m_pos.block).arg(m_pos.offset);

    return 0;
}

TypedFile* DocxPlugin::parse()
{
    QFile f(m_pos.dev);
    if (!f.open(QIODevice::ReadOnly))
    {
        f.close();
        return 0;
    }

    f.seek(m_pos.block * m_pos.block_size + m_pos.offset);
    QByteArray arr;
    for (long long unsigned i=0; i < FILESIZE / MAXREAD; ++i)
        arr.append(f.read(MAXREAD));

    // looking in docx only
    foreach(const QByteArray &trailer, trailers)
    {
        int pos = arr.indexOf(trailer);
        if (pos != -1)
        {
            arr = arr.left(pos + m_trailerLen + trailer.length());
            break;
        }
    }

    QFile newf(TMPFILENAME);
    TypedFile* file=0;

    if (newf.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        newf.write(arr);
        newf.close();

        QString mnemonics;
        QProcess exporter;
        QTemporaryFile tempFile;

        if (isDocx(arr))
        {
            exporter.start("docx2txt", QStringList() << TMPFILENAME << "-");  // Now parse tmp file with docx2txt
            mnemonics = "DOCX";
        }

        else if (isXlsx(arr))
        {
            if (tempFile.open())
            {
                exporter.start("xlsx2csv.py", QStringList() << "-d" << "tab" << TMPFILENAME << tempFile.fileName());  // Now parse tmp file with xls2csv
                mnemonics = "XLSX";
            }
        }

        if (!mnemonics.isEmpty() && exporter.waitForStarted())
        {
            exporter.waitForFinished(500);
            QByteArray result;

            if (mnemonics != "XLSX")
                result = exporter.readAll();

            else
                result = tempFile.readAll();

            file = new TypedFile;
            file->buf = new QByteArray(result);
            TypedFindPosition* p = new TypedFindPosition;
            *p = m_pos;
            p->type = mnemonics;
            file->pos = p;
        }

        QFile::remove(TMPFILENAME);
    }
    f.close();
    return file;
}

Q_EXPORT_PLUGIN2(docxplugin, DocxPlugin);
