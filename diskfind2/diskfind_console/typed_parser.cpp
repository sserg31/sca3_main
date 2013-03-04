#include "typed_parser.h"

QVector<TypedFile*>* ParserCreator::search()
{
    /*const QString dev,
              const long long unsigned block,
              const long long unsigned offset,
              const unsigned int block_size*

    return 0;*/
    return parse_buffer(buf);
}

QVector<TypedFile*>* ParserCreator::parse_buffer(const QByteArray& buf)
{
    QVector<TypedFile*>* vec = new QVector<TypedFile*>;

    TypedFindPosition SigPos;
    SigPos.block = block;
    SigPos.block_size = blocksize;
    SigPos.dev = dev;
    foreach (const FileSigs &vsig, *vsigs)
    {
        int pos = buf.indexOf(vsig.header,0);
        if (pos != -1)
        {
            SigPos.offset = pos;
            SigPos.type = vsig.mnemonics;
            BaseTypedParser* tp = vsig.interface;

            if (tp){
                tp->set_params(SigPos);
                TypedFile* tmp = tp->parse();
                if (tmp)
                    vec->append(tmp);
            }
        }
    }

    if (vec->isEmpty())
    {
        delete vec;
        vec = 0;
    }
    return vec;
}

