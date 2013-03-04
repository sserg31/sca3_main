#ifndef FATFSPLUGIN_H
#define FATFSPLUGIN_H

#include <QObject>

#include "basefsparser.h"

#include "ntfsundelete.h"

class NtfsPlugin : public QObject, BaseFSParser
{
    Q_OBJECT
    Q_INTERFACES(BaseFSParser)

public:
    NtfsPlugin();
    virtual ~NtfsPlugin();

    /// returns true plugin can handle filesystem type
    virtual bool fsmatch(const QString &type);

    /// returns filename by offset from beginning
    virtual QString getFilename(quint64 offset);

    /// reads system info structures
    virtual int prepare(const QString &tmpPath, const QString &device);

    /// closes some handles if necessary
    virtual int finalize();

private:
    ntfs_volume *m_vol;

    myfile **m_clusters;
    myfile **m_mftRecords;

    unsigned int m_mftRecordsInCluster;

    QString m_tmpMftFilename;
    int m_mftFd;
    size_t m_mftLen;
    quint64 m_mftRecordsNum;

    FILE_NAME_ATTR* verify_parent(struct filename* name, MFT_RECORD* rec);
    int get_parent_name(struct filename* name, ntfs_volume* m_vol, long long *parent_inode);
    int get_filenames(struct ufile *file, ntfs_volume* m_vol, QStringList &names);
    int get_data(struct ufile *file, ntfs_volume *m_vol);

    void free_file(struct ufile *file);
    struct ufile *read_record(ntfs_volume *m_vol, long long record, QStringList &names);
    void clusters_file(ntfs_volume *m_vol, long long inode);
    int scan_disk(ntfs_volume *m_vol);
};

#endif
