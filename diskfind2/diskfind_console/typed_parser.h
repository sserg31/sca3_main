#ifndef TYPED_PARSER_H
#define TYPED_PARSER_H

#include <iostream>
#include <QRegExp>
#include <QVector>

#include <signature.h>

#include "support.h"
#include "../diskfind_plugins/basetypedparser.h"

using namespace std;

class ParserCreator
{
public:
    ParserCreator(const QString& dev,
                  const QByteArray& buf,
                  long long unsigned block,
                  unsigned int blocksize,
                  const QVector<FileSigs>* sigs)
    {
        this->block = block;
        this->blocksize = blocksize;
        this->buf = buf;
        this->dev = dev;
        this->vsigs = sigs;
    }

    QVector<TypedFile*>* search();

private:

    QVector<TypedFile*>* parse_buffer(const QByteArray& buf);


    long long unsigned block;
    unsigned int blocksize;
    QByteArray buf;
    QString dev;
    const QVector<FileSigs>* vsigs;
};

#endif // TYPED_PARSER_H
