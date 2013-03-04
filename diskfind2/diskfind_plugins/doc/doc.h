#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include <QObject>
#include <QProcess>

#include <iostream>

#include "basetypedparser.h"


using namespace std;

class DocPlugin : public QObject, BaseTypedParser
{
    Q_OBJECT
    Q_INTERFACES(BaseTypedParser)

public:
    DocPlugin();
    QByteArray get_signature();
    QString get_mnemonics();
    int set_params(const TypedFindPosition& pos);
    TypedFile* parse();

private:
    bool isDoc(const QByteArray &data) const;
    bool isXls(const QByteArray &data) const;
};

#endif
