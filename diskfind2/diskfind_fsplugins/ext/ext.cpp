
#include <QDir>
#include <QtPlugin>

#include "ext.h"

ExtPlugin::ExtPlugin()
{

}

ExtPlugin::~ExtPlugin()
{

}

int ExtPlugin::lookup_blocks(ext2_filsys fs,
                         blk_t *block_nr,
                         int blockcnt EXT2FS_ATTR((unused)),
                         void *priv_data EXT2FS_ATTR((unused)))
{
    if (!*block_nr)
        return 0;

    struct lookup_inodes_struct *ls = (struct lookup_inodes_struct *) priv_data;

    if (*block_nr < fs->super->s_blocks_count &&  *block_nr >= fs->super->s_first_data_block)
        m_blocks[*block_nr] = ls->filename;

    return 0;
}

int ExtPlugin::lookup_blocks_wrapper(ext2_filsys fs,
                         blk_t *block_nr,
                         int blockcnt EXT2FS_ATTR((unused)),
                         void *priv_data EXT2FS_ATTR((unused)))
{
    if (!*block_nr)
            return 0;

    struct lookup_inodes_struct *ls = (struct lookup_inodes_struct *) priv_data;

    return ls->plugin->lookup_blocks(fs, block_nr, blockcnt, priv_data);
}

int ExtPlugin::lookup_inodes_wrapper(struct ext2_dir_entry *dirent,
                       int      offset EXT2FS_ATTR((unused)),
                       int      blocksize EXT2FS_ATTR((unused)),
                       char     *buf EXT2FS_ATTR((unused)),
                       void     *priv_data)
{
    struct lookup_inodes_struct *ls = (struct lookup_inodes_struct *) priv_data;

    return ls->plugin->lookup_inodes(dirent, offset, blocksize, buf, priv_data);
}

int ExtPlugin::lookup_inodes(struct ext2_dir_entry *dirent,
                       int      offset EXT2FS_ATTR((unused)),
                       int      blocksize EXT2FS_ATTR((unused)),
                       char     *buf EXT2FS_ATTR((unused)),
                       void     *priv_data)
{
    struct lookup_inodes_struct *ls = (struct lookup_inodes_struct *) priv_data;
    ext2_inode inode;

    ext2fs_read_inode(ls->plugin->m_fs, dirent->inode, &inode);

    if (LINUX_S_ISREG(inode.i_mode) || LINUX_S_ISLNK(inode.i_mode))
    {
        ls->filename = getFilenameByInode(ls->dirInode, dirent->inode);
        // iterate over all blocks in inode
        ext2fs_block_iterate(ls->plugin->m_fs, dirent->inode, 0, 0, ExtPlugin::lookup_blocks_wrapper, ls);
    }

    return 0;
}

char* ExtPlugin::getFilenameByInode(ext2_ino_t dirInode, ext2_ino_t fileInode)
{
    char *name = 0;

    ext2fs_get_pathname(m_fs, dirInode, fileInode, &name);

    return name;
}

errcode_t ExtPlugin::getFilenameByInodes(ext2_ino_t dirInode)
{
    struct lookup_inodes_struct ls;
    ls.dirInode = dirInode;
    ls.filename = NULL;
    ls.plugin = this;

    errcode_t retval = ext2fs_dir_iterate(m_fs, dirInode, 0, 0, ExtPlugin::lookup_inodes_wrapper, &ls);

    return retval;
}

QString ExtPlugin::getFilename(quint64 offset)
{
    unsigned long long block_size = 1024 << m_fs->super->s_log_block_size;
    unsigned long long block = offset / block_size;

    if (block > m_fs->super->s_blocks_count)
        return QString();

    return QString::fromUtf8(m_blocks[block]);
}

int ExtPlugin::prepare(const QString &tmpPath, const QString &device)
{
    const char *dev = device.toUtf8().constData();
    errcode_t retval = ext2fs_open(dev, 0, 0, 0, unix_io_manager, &m_fs);
    if (retval)
    {
        fprintf(stderr, "%ld %s %s", retval, "while trying to open %s", dev);
        return 0;
    }

    m_len = m_fs->super->s_blocks_count * sizeof(char*);
    m_tmpFilename = QDir(tmpPath).absoluteFilePath(device.mid(device.lastIndexOf('/') + 1));

    if (!filePrepare(m_tmpFilename, m_len, &m_fd))
        return 0;

    if (!map((void**)&m_blocks, m_len, m_fd))
        return 0;

    memset(m_blocks, 0, m_len);

    if (scanDisk())
        return 0;

    return 1;
}

errcode_t ExtPlugin::scanDisk()
{
    ext2_inode_scan scan;
    ext2_ino_t	ino;
    struct ext2_inode inode;

    errcode_t retval = ext2fs_open_inode_scan(m_fs, 0, &scan);

    if (retval)
    {
        fprintf(stderr, "%ld %s", retval, "while opening inode scan");
        return retval;
    }

    retval = ext2fs_get_next_inode(scan, &ino, &inode);
    if (retval)
    {
        fprintf(stderr, "%ld %s", retval, "while starting inode scan");
        return retval;
    }

    while (ino)
    {
        if (inode.i_links_count && LINUX_S_ISDIR(inode.i_mode))
            getFilenameByInodes(ino);

        retval = ext2fs_get_next_inode(scan, &ino, &inode);
        if (retval)
        {
            fprintf(stderr, "%ld %s", retval, "while doing inode scan");
            return retval;
        }
    }
    return retval;
}

int ExtPlugin::finalize()
{
    ext2fs_close(m_fs);

    fileFinalyze(m_blocks, m_len, m_fd);

    if (!QFile::remove(m_tmpFilename))
        perror("Error removing temporary file");

    return 0;
}

bool ExtPlugin::fsmatch(const QString &type)
{
    QStringList list = QStringList() << "ext2" << "ext3" << "ext4";

    return list.contains(type, Qt::CaseInsensitive);
}

Q_EXPORT_PLUGIN2(ExtPlugin, ExtPlugin);
