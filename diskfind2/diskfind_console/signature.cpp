#include "signature.h"

extern bool quiet;

void PluginsLoader::load()
{
    sigs->clear();
    BaseTypedParser *base;
    QDir pluginsDir(QCoreApplication::applicationDirPath() + "/../share/diskfind/plugins/");

    if (!quiet)
        cout << "searching for plugins in " << pluginsDir.absolutePath().toUtf8().constData() << std::endl;

    pluginsDir.cd(loaddir);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            if (!quiet)
                cout << "plugin loaded: " << pluginsDir.absoluteFilePath(fileName).toUtf8().constData() << std::endl;

            base = qobject_cast<BaseTypedParser *>(plugin);
            FileSigs tmp;
            tmp.interface = base;
            tmp.header = base->get_signature();
            tmp.mnemonics = base->get_mnemonics();
            sigs->append(tmp);
        }
    }

    if (!quiet)
        cout << "sigs size: " << sigs->size() << endl;
}

void PluginsLoader::loadFS()
{
    m_fssigs.clear();
    BaseFSParser *base;
    QDir pluginsDir(QCoreApplication::applicationDirPath() + "/../share/diskfind/fsplugins/");

    if (!quiet)
        cout << "searching for filesystem plugins in " << pluginsDir.absolutePath().toUtf8().constData() << std::endl;

    pluginsDir.cd(loaddir);
    foreach (const QString &fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            if (!quiet)
                cout << "plugin loaded: " << pluginsDir.absoluteFilePath(fileName).toUtf8().constData() << std::endl;

            base = qobject_cast<BaseFSParser *>(plugin);

            m_fssigs.append(base);
        }

        else
            cout << "error loading plugin: " << pluginsDir.absoluteFilePath(fileName).toUtf8().constData() << pluginLoader.errorString().toUtf8().constData() << std::endl;
    }

    if (!quiet)
        cout << "fs sigs size: " << m_fssigs.size() << endl;
}

QVector<FileSigs>* PluginsLoader::get_sigs(const QStringList& types)
{
    QVector<FileSigs>* ns = new QVector<FileSigs>;
    foreach(const FileSigs &sig, *sigs)
    {
        if (types.contains(sig.mnemonics, Qt::CaseInsensitive))
            ns->append(sig);
    }

    if (ns->isEmpty())
    {
        delete ns;
        ns = 0;
    }

    return ns;
}

BaseFSParser* PluginsLoader::getFSParser(const QString &type)
{
    foreach(BaseFSParser *parser, m_fssigs)
    {
        if (parser->fsmatch(type))
            return parser;
    }

    return NULL;
}
