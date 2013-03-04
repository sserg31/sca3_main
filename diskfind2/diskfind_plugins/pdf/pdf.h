#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include <QObject>
#include <QProcess>

#include <iostream>

#include "basetypedparser.h"


using namespace std;

class PdfPlugin : public QObject, BaseTypedParser
{
    Q_OBJECT
    Q_INTERFACES(BaseTypedParser)

public:
    PdfPlugin();
    QByteArray get_signature();
    QString get_mnemonics();
    int set_params(const TypedFindPosition& pos);
    TypedFile* parse();

private:
    static const QList<QByteArray> trailers;
};

#endif
