#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include <QString>
#include <QVector>
#include <QByteArray>
#include <QtPlugin>

#include "../diskfind_console/support.h"

class BaseTypedParser
{
public:
    virtual ~BaseTypedParser() {}
    virtual QByteArray get_signature() = 0;
    virtual QString get_mnemonics() = 0;
    virtual TypedFile* parse() = 0;
    virtual int set_params(const TypedFindPosition& pos) = 0;

protected:
    long long unsigned FILESIZE;
    QString TMPFILENAME;
    unsigned MAXREAD;
    TypedFindPosition m_pos;
};

Q_DECLARE_INTERFACE(BaseTypedParser, "com.trolltech.Plugin.BaseTypedParser/1.0");

#endif
