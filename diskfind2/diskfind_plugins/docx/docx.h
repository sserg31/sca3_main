#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include <QObject>
#include <QProcess>

#include <iostream>

#include "basetypedparser.h"


using namespace std;

class DocxPlugin : public QObject, BaseTypedParser
{
    Q_OBJECT
    Q_INTERFACES(BaseTypedParser)

public:
    DocxPlugin();
    QByteArray get_signature();
    QString get_mnemonics();
    int set_params(const TypedFindPosition& pos);
    TypedFile* parse();

private:
    static const int m_trailerLen = 0x12;
    static const QList<QByteArray> trailers;

    int OOXMLRootPos(const QByteArray &data) const;

    bool isDocx(const QByteArray &data) const;
    bool isXlsx(const QByteArray &data) const;
};

#endif
