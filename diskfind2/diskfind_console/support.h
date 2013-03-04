#ifndef SUPPORT_H
#define SUPPORT_H

#include <QTextCodec>

struct SearchResult
{
    long long unsigned block;
    int offset;
    QString str;
    QString enc;
    QString resulttype;
    QString filepath;
};

struct TypedFindPosition
{
    QString type;
    QString dev;
    long long unsigned block;
    long long unsigned offset;
    unsigned int block_size;
};

struct TypedFile
{
    TypedFindPosition* pos;
    QByteArray* buf;
};

QTextCodec* get_codec(const QString& str);
#endif // SUPPORT_H
