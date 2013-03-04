#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <basetypedparser.h>
#include <support.h>

#include <iostream>

using namespace std;

class DocParser : public BaseTypedParser
{
public:
    DocParser(const TypedFindPosition& pos);
    QVector<SearchResult*>* parse();
private:
    QString dev;
    long long unsigned block;
    long long unsigned offset;
    unsigned int block_size;

    long long unsigned FILESIZE;
    QString TMPFILENAME;
};

#endif // DOCPARSER_H
