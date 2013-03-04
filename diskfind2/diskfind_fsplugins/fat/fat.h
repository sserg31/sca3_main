#ifndef FATFSPLUGIN_H
#define FATFSPLUGIN_H

#include <QObject>

#include "basefsparser.h"
#include "dosfsck.h"

class FatPlugin : public QObject, BaseFSParser
{
    Q_OBJECT
    Q_INTERFACES(BaseFSParser)

public:
    FatPlugin();
    virtual ~FatPlugin();

    /// returns true plugin can handle filesystem type
    virtual bool fsmatch(const QString &type);

    /// returns filename by offset from beginning
    virtual QString getFilename(quint64 offset);

    /// reads system info structures
    virtual int prepare(const QString &tmpPath, const QString &device);

    /// closes some handles if necessary
    virtual int finalize();

private:
    DOS_FS m_fs;

    char *path_name(DOS_FILE *file);
};

#endif
