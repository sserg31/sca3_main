#include "support.h"
QTextCodec* get_codec(const QString& str)
{
    //Use to convert words from dict to unicode( UTF-8 by default );
    return QTextCodec::codecForName(str.toAscii());
}
