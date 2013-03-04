#include <QFile>
#include <QSet>

#include "doc.h"

DocPlugin::DocPlugin()
{
    FILESIZE = 5*1024*1024;
    MAXREAD = 16*1024;
}

bool DocPlugin::isDoc(const QByteArray &data) const
{
    static const char subsign[] = {0xEC, 0xA5, 0xC1, 0x00};

    return data.indexOf(subsign) == 512;
}

bool DocPlugin::isXls(const QByteArray &data) const
{
    static const char subsign1[] = {0x09, 0x08, 0x10, 0x00, 0x00, 0x06, 0x05, 0x00};
    static const char subsign2[] = {0xFD, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00};
    static const char subsignpart[] = {0xFD, 0xFF, 0xFF, 0xFF};
    static const QSet<int> nn = QSet<int>() << 0x10 << 0x1F << 0x22 << 0x23 << 0x28 << 0x29;

    if (data.indexOf(subsign1) == 512 || data.indexOf(subsign2) == 512)
        return true;

    int pos;
    if ((pos = data.indexOf(subsignpart)) == 512)
    {
        int len = sizeof(subsignpart);
        char nexttolast = data.at(pos + len);
        char last = data.at(pos + len + 1);

        if (last == 0x00 || last == 0x02)
            return nn.contains(nexttolast);
    }
    return false;
}

QByteArray DocPlugin::get_signature()
{
    static const char s[] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    return QByteArray(s, sizeof(s));
}

QString DocPlugin::get_mnemonics()
{
    return "DOC";
}

int DocPlugin::set_params(const TypedFindPosition& pos)
{
    m_pos = pos;
    TMPFILENAME = QString("/tmp/sign.%1.%2").arg(m_pos.block).arg(m_pos.offset);

    return 0;
}

TypedFile* DocPlugin::parse()
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

    QFile newf(TMPFILENAME);
    TypedFile* file=0;

    if (newf.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        newf.write(arr);
        newf.close();

        QString mnemonics;
        QProcess exporter;
        if (isDoc(arr))
        {
            exporter.start("catdoc", QStringList() << TMPFILENAME);  // Now parse tmp file with catdoc
            mnemonics = "DOC";
        }

        else if (isXls(arr))
        {
            exporter.start("xls2csv", QStringList() << TMPFILENAME);  // Now parse tmp file with xls2csv
            mnemonics = "XLS";
        }

        if (!mnemonics.isEmpty() && exporter.waitForStarted())
        {
            exporter.waitForFinished(500);
            QByteArray result = exporter.readAll();
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

Q_EXPORT_PLUGIN2(docplugin, DocPlugin);
