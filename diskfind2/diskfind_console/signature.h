#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QPluginLoader>
#include <QCoreApplication>

#include "../diskfind_plugins/basetypedparser.h"
#include "../diskfind_fsplugins/basefsparser.h"
#include <iostream>

using namespace std;

struct FileSigs
{
    QByteArray header;
    QString mnemonics;
    BaseTypedParser* interface;
};

class PluginsLoader
{
public:
    PluginsLoader(const QString& loaddir)
    {
        this->loaddir = loaddir;
        sigs = new QVector<FileSigs>;
    }

    ~PluginsLoader()
    {
        qDeleteAll(m_fssigs);
        delete sigs;
    }

    void load();
    void loadFS();
    QVector<FileSigs>* get_sigs(const QStringList& types);
    BaseFSParser *getFSParser(const QString &type);

private:
    QString loaddir;
    QVector<FileSigs>* sigs;
    QVector<BaseFSParser*> m_fssigs;
};

#endif // SIGNATURE_H
