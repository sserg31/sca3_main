#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include <QObject>
#include <QProcess>

#include <iostream>

#include "basetypedparser.h"


using namespace std;

class OdtPlugin : public QObject, BaseTypedParser
{
    Q_OBJECT
    Q_INTERFACES(BaseTypedParser)

public:
    OdtPlugin();
    QByteArray get_signature();
    QString get_mnemonics();
    int set_params(const TypedFindPosition& pos);
    TypedFile* parse();

private:
    static const int m_trailerLen = 0x2b;
    static const QList<QByteArray> trailers;

    QString determineMnenomics(const QByteArray &data) const;
};

#endif
