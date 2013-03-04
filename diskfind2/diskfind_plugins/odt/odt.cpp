
#include <QFile>

#include "odt.h"

//META-INF/manifest.xmlPK
static const char m_trailer[] = { 0x4d, 0x45, 0x54, 0x41, 0x2d, 0x49, 0x4e, 0x46, 0x2f, 0x6d, 0x61, 0x6e, 0x69, 0x66, 0x65, 0x73, 0x74, 0x2e, 0x78, 0x6d, 0x6c, 0x50, 0x4b, 0x05, 0x06 };

const QList<QByteArray> OdtPlugin::trailers = QList<QByteArray>() << QByteArray(m_trailer, sizeof(m_trailer));

OdtPlugin::OdtPlugin()
{
    FILESIZE = 5*1024*1024;
    MAXREAD = 16*1024;
}

QByteArray OdtPlugin::get_signature()
{
    static const char s[] = { 0x50, 0x4B, 0x03, 0x04 };
    return QByteArray(s, 4);
}

QString OdtPlugin::get_mnemonics()
{
    return "ODT";
}

QString OdtPlugin::determineMnenomics(const QByteArray &data) const
{
    int mimeOffset = 0x1E;
    QByteArray tmp = data.mid(mimeOffset);
    static char mimeBegin[] = "mimetypeapplication/vnd.oasis.opendocument.";

    if (tmp.startsWith(mimeBegin))
        return QString("OD%1").arg(QChar(tmp.at(sizeof(mimeBegin)-1)).toUpper());

    else
        return "OD*";
}

int OdtPlugin::set_params(const TypedFindPosition& pos)
{
    m_pos = pos;
    TMPFILENAME = QString("/tmp/sign.%1.%2").arg(m_pos.block).arg(m_pos.offset);

    return 0;
}

TypedFile* OdtPlugin::parse()
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

    // looking in odt only
    foreach(const QByteArray &trailer, trailers)
    {
        int pos = arr.indexOf(trailer);
        if (pos != -1)
        {
            arr = arr.left(pos+m_trailerLen);
            break;
        }
    }

    QFile newf(TMPFILENAME);
    TypedFile* file=0;
    if (newf.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        newf.write(arr);
        newf.close();

        // Now parse tmp file with pdftotext
        QProcess odt2txt;
        odt2txt.start("odt2txt", QStringList() << TMPFILENAME);
        if (odt2txt.waitForStarted())
        {
            odt2txt.waitForFinished(500);

            if (!odt2txt.exitCode())
            {
                QByteArray result = odt2txt.readAll();
                file = new TypedFile;
                file->buf = new QByteArray(result);

                TypedFindPosition* p = new TypedFindPosition;
                *p = m_pos;
                p->type = determineMnenomics(arr);
                file->pos = p;
            }
        }
        QFile::remove(TMPFILENAME);
    }
    f.close();
    return file;
}

Q_EXPORT_PLUGIN2(odtplugin, OdtPlugin);
