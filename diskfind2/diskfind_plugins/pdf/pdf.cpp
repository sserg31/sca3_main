
#include <QFile>

#include "pdf.h"

static const char trailer1[] = { 0x0D, 0x0A, 0x25, 0x25, 0x45, 0x4F, 0x46, 0x0D, 0x0A };
static const char trailer2[] = { 0x0A, 0x25, 0x25, 0x45, 0x4F, 0x46, 0x0A };
static const char trailer3[] = { 0x0D, 0x25, 0x25, 0x45, 0x4F, 0x46, 0x0D };

const QList<QByteArray> PdfPlugin::trailers = QList<QByteArray>() << QByteArray(trailer1, sizeof(trailer1))
                                                                  << QByteArray(trailer2, sizeof(trailer2))
                                                                  << QByteArray(trailer3, sizeof(trailer3));

PdfPlugin::PdfPlugin()
{
    FILESIZE = 5*1024*1024;
    MAXREAD = 16*1024;
}

QByteArray PdfPlugin::get_signature()
{
    static const char s[] = { 0x25, 0x50, 0x44, 0x46 };
    return QByteArray(s, sizeof(s));
}

QString PdfPlugin::get_mnemonics()
{
    return "PDF";
}

int PdfPlugin::set_params(const TypedFindPosition& pos)
{
    m_pos = pos;
    TMPFILENAME = QString("/tmp/sign.%1.%2").arg(m_pos.block).arg(m_pos.offset);

    return 0;
}

TypedFile* PdfPlugin::parse()
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

    // looking in pdf only
    foreach(const QByteArray &trailer, trailers)
    {
        int pos = arr.indexOf(trailer);
        if (pos != -1)
        {
            arr = arr.left(pos+trailer.length());
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
        QProcess pdftotext;
        pdftotext.start("pdftotext", QStringList() << TMPFILENAME << "-"); // need output to stdout
        if (pdftotext.waitForStarted())
        {
            pdftotext.waitForFinished(500);
            QByteArray result = pdftotext.readAll();
            file = new TypedFile;
            file->buf = new QByteArray(result);

            TypedFindPosition* p = new TypedFindPosition;
            *p = m_pos;
            p->type = get_mnemonics();
            file->pos = p;
        }
        QFile::remove(TMPFILENAME);
    }
    f.close();
    return file;
}

Q_EXPORT_PLUGIN2(pdfplugin, PdfPlugin);
