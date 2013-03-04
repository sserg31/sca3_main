#ifndef FATFSPLUGIN_H
#define FATFSPLUGIN_H

#include <QObject>

#include "basefsparser.h"

#include <ext2fs.h>

class ExtPlugin : public QObject, BaseFSParser
{
    Q_OBJECT
    Q_INTERFACES(BaseFSParser)

public:
    ExtPlugin();
    virtual ~ExtPlugin();

    /// returns true plugin can handle filesystem type
    virtual bool fsmatch(const QString &type);

    /// returns filename by offset from beginning
    virtual QString getFilename(quint64 offset);

    /// reads system info structures
    virtual int prepare(const QString &tmpPath, const QString &device);

    /// closes some handles if necessary
    virtual int finalize();

private:
    struct lookup_inodes_struct
    {
        ext2_ino_t dirInode;
        ExtPlugin *plugin;
        char *filename;
    };

    ext2_filsys m_fs;
    char **m_blocks;

    errcode_t scanDisk();

    errcode_t getFilenameByInodes(ext2_ino_t dirInode);
    char* getFilenameByInode(ext2_ino_t dirInode, ext2_ino_t fileInode);

    int lookup_blocks(ext2_filsys m_fs,
                                        blk_t *block_nr,
                                        int blockcnt EXT2FS_ATTR((unused)),
                                        void *priv_data EXT2FS_ATTR((unused)));

    static int lookup_blocks_wrapper(ext2_filsys m_fs,
                                        blk_t *block_nr,
                                        int blockcnt EXT2FS_ATTR((unused)),
                                        void *priv_data EXT2FS_ATTR((unused)));

    int lookup_inodes(struct ext2_dir_entry *dirent,
                                        int      offset EXT2FS_ATTR((unused)),
                                        int      blocksize EXT2FS_ATTR((unused)),
                                        char     *buf EXT2FS_ATTR((unused)),
                                        void     *priv_data);
    static int lookup_inodes_wrapper(struct ext2_dir_entry *dirent,
                                        int      offset EXT2FS_ATTR((unused)),
                                        int      blocksize EXT2FS_ATTR((unused)),
                                        char     *buf EXT2FS_ATTR((unused)),
                                        void     *priv_data);
};

#endif
