#include <QStringList>
#include <QDir>

#include <fcntl.h>
#include <sys/mman.h>

#include "ntfs.h"

extern "C" {
#include <utils.h>
#include <ntfstime.h>
}

NtfsPlugin::NtfsPlugin()
{

}

NtfsPlugin::~NtfsPlugin()
{

}

/**
 * verify_parent - confirm a record is parent of a file
 * @name:	a filename of the file
 * @rec:	the mft record of the possible parent
 *
 * Check that @rec is the parent of the file represented by @name.
 * If @rec is a directory, but it is created after @name, then we
 * can't determine whether @rec is really @name's parent.
 *
 * Return:	@rec's filename, either same name space as @name or lowest space.
 *		NULL if can't determine parenthood or on error.
 */
FILE_NAME_ATTR* NtfsPlugin::verify_parent(struct filename* name, MFT_RECORD* rec)
{
        ATTR_RECORD *attr30;
        FILE_NAME_ATTR *filename_attr = NULL, *lowest_space_name = NULL;
        ntfs_attr_search_ctx *ctx;
        int found_same_space = 1;

        if (!name || !rec)
                return NULL;

        if (!(rec->flags & MFT_RECORD_IS_DIRECTORY)) {
                return NULL;
        }

        ctx = ntfs_attr_get_search_ctx(NULL, rec);
        if (!ctx) {
                ntfs_log_error("ERROR: Couldn't create a search context.\n");
                return NULL;
        }

        attr30 = find_attribute(AT_FILE_NAME, ctx);
        if (!attr30) {
                return NULL;
        }

        filename_attr = (FILE_NAME_ATTR*)((char*)attr30 + le16_to_cpu(attr30->value_offset));
        /* if name is older than this dir -> can't determine */
        if (ntfs2utc(filename_attr->creation_time) > name->date_c) {
                return NULL;
        }

        if (filename_attr->file_name_type != name->name_space) {
                found_same_space = 0;
                lowest_space_name = filename_attr;

                while (!found_same_space && (attr30 = find_attribute(AT_FILE_NAME, ctx))) {
                        filename_attr = (FILE_NAME_ATTR*)((char*)attr30 + le16_to_cpu(attr30->value_offset));

                        if (filename_attr->file_name_type == name->name_space) {
                                found_same_space = 1;
                        } else {
                                if (filename_attr->file_name_type < lowest_space_name->file_name_type) {
                                        lowest_space_name = filename_attr;
                                }
                        }
                }
        }

        ntfs_attr_put_search_ctx(ctx);

        return (found_same_space ? filename_attr : lowest_space_name);
}

/**
 * get_parent_name - Find the name of a file's parent.
 * @name:	the filename whose parent's name to find
 *
 * Return:	1 if has parent
 *		0 is hasn't parent
 */
int NtfsPlugin::get_parent_name(struct filename* name, ntfs_volume* vol, long long *parent_inode)
{
        ntfs_attr* mft_data;
        MFT_RECORD* rec;
        FILE_NAME_ATTR* filename_attr;
        long long inode_num;
        int res = 1;

        if (!name || !vol)
                return 0;

        rec = (MFT_RECORD*)calloc(1, vol->mft_record_size);
        if (!rec) {
                ntfs_log_error("ERROR: Couldn't allocate memory in "
                                "get_parent_name()\n");
                return 0;
        }

        mft_data = ntfs_attr_open(vol->mft_ni, AT_DATA, AT_UNNAMED, 0);
        if (!mft_data) {
                ntfs_log_perror("ERROR: Couldn't open $MFT/$DATA");
        } else {
                inode_num = MREF_LE(name->parent_mref);

                if (ntfs_attr_pread(mft_data, vol->mft_record_size * inode_num,
                                        vol->mft_record_size, rec) < 1) {
                        ntfs_log_error("ERROR: Couldn't read MFT Record %lld"
                                        ".\n", inode_num);
                } else if ((filename_attr = verify_parent(name, rec))) {
                        if (ntfs_ucstombs(filename_attr->file_name,
                                        filename_attr->file_name_length,
                                        &name->parent_name, 0) < 0) {
                                ntfs_log_debug("ERROR: Couldn't translate "
                                                "filename to current "
                                                "locale.\n");
                                name->parent_name = NULL;
                        }
                }
        }

        if (mft_data) {
                ntfs_attr_close(mft_data);
        }

        if (rec) {
                free(rec);
        }

        if (*parent_inode == inode_num)
            return 0;
        *parent_inode = inode_num;
        return res;
}

/**
 * get_filenames - Read an MFT Record's $FILENAME attributes
 * @file:  The file object to work with
 *
 * A single file may have more than one filename.  This is quite common.
 * Windows creates a short DOS name for each long name, e.g. LONGFI~1.XYZ,
 * LongFiLeName.xyZ.
 *
 * The filenames that are found are put in filename objects and added to a
 * linked list of filenames in the file object.  For convenience, the unicode
 * filename is converted into the current locale and stored in the filename
 * object.
 *
 * One of the filenames is picked (the one with the lowest numbered namespace)
 * and its locale friendly name is put in pref_name.
 *
 * Return:  n  The number of $FILENAME attributes found
 *	   -1  Error
 */
int NtfsPlugin::get_filenames(struct ufile *file, ntfs_volume* vol, QStringList &names)
{
        ATTR_RECORD *rec;
        FILE_NAME_ATTR *attr;
        ntfs_attr_search_ctx *ctx;
        struct filename *name;
        int count = 0;
        int space = 4;
        bool parent_search = true;
        long long parent_inode;

        if (!file)
                return -1;

        ctx = ntfs_attr_get_search_ctx(NULL, file->mft);
        if (!ctx)
                return -1;

        while ((rec = find_attribute(AT_FILE_NAME, ctx))) {
                /* We know this will always be resident. */
                attr = (FILE_NAME_ATTR *)((char *)rec +
                                le16_to_cpu(rec->value_offset));

                name = (filename*)calloc(1, sizeof(*name));
                if (!name) {
                        ntfs_log_error("ERROR: Couldn't allocate memory in "
                                        "get_filenames().\n");
                        count = -1;
                        break;
                }

                name->uname      = attr->file_name;
                name->uname_len  = attr->file_name_length;
                name->name_space = attr->file_name_type;
                name->size_alloc = sle64_to_cpu(attr->allocated_size);
                name->size_data  = sle64_to_cpu(attr->data_size);
                name->flags      = attr->file_attributes;

                name->date_c     = ntfs2utc(attr->creation_time);
                name->date_a     = ntfs2utc(attr->last_data_change_time);
                name->date_m     = ntfs2utc(attr->last_mft_change_time);
                name->date_r     = ntfs2utc(attr->last_access_time);

                if (ntfs_ucstombs(name->uname, name->uname_len, &name->name,
                                0) < 0) {
                        ntfs_log_debug("ERROR: Couldn't translate filename to "
                                        "current locale.\n");
                }

                name->parent_name = NULL;

                name->parent_mref = attr->parent_directory;
                parent_inode = file->inode;
                while (get_parent_name(name, vol, &parent_inode) && parent_search)
                    read_record(vol, parent_inode, names);

                parent_search = false;

                if (name->name_space < space) {
                        file->pref_name = name->name;
                        file->pref_pname = name->parent_name;
                        space = name->name_space;
                }

                file->max_size = max(file->max_size, name->size_alloc);
                file->max_size = max(file->max_size, name->size_data);

                list_add_tail(&name->list, &file->name);
                count++;
        }

        if (file->pref_name)
            names.append(QString::fromUtf8(file->pref_name));

        ntfs_attr_put_search_ctx(ctx);
        ntfs_log_debug("File has %d names.\n", count);
        return count;
}

/**
 * get_data - Read an MFT Record's $DATA attributes
 * @file:  The file object to work with
 * @vol:  An ntfs volume obtained from ntfs_mount
 *
 * A file may have more than one data stream.  All files will have an unnamed
 * data stream which contains the file's data.  Some Windows applications store
 * extra information in a separate stream.
 *
 * The streams that are found are put in data objects and added to a linked
 * list of data streams in the file object.
 *
 * Return:  n  The number of $FILENAME attributes found
 *	   -1  Error
 */
int NtfsPlugin::get_data(struct ufile *file, ntfs_volume *vol)
{
        ATTR_RECORD *rec;
        ntfs_attr_search_ctx *ctx;
        int count = 0;
        struct data *data;

        if (!file)
                return -1;

        ctx = ntfs_attr_get_search_ctx(NULL, file->mft);
        if (!ctx)
                return -1;

        while ((rec = find_attribute(AT_DATA, ctx))) {
                data = (struct data*)calloc(1, sizeof(*data));
                if (!data) {
                        ntfs_log_error("ERROR: Couldn't allocate memory in "
                                        "get_data().\n");
                        count = -1;
                        break;
                }

                data->resident   = !rec->non_resident;
                data->compressed = (rec->flags & ATTR_IS_COMPRESSED) ? 1 : 0;
                data->encrypted  = (rec->flags & ATTR_IS_ENCRYPTED) ? 1 : 0;

                if (rec->name_length) {
                        data->uname = (ntfschar *)((char *)rec +
                                        le16_to_cpu(rec->name_offset));
                        data->uname_len = rec->name_length;

                        if (ntfs_ucstombs(data->uname, data->uname_len,
                                                &data->name, 0) < 0) {
                                ntfs_log_error("ERROR: Cannot translate name "
                                                "into current locale.\n");
                        }
                }

                if (data->resident) {
                        data->size_data = le32_to_cpu(rec->value_length);
                        data->data = (char*)rec +
                                le16_to_cpu(rec->value_offset);
                } else {
                        data->size_alloc = sle64_to_cpu(rec->allocated_size);
                        data->size_data  = sle64_to_cpu(rec->data_size);
                        data->size_init  = sle64_to_cpu(rec->initialized_size);
                        data->size_vcn   = sle64_to_cpu(rec->highest_vcn) + 1;
                }

                data->runlist = ntfs_mapping_pairs_decompress(vol, rec, NULL);
                if (!data->runlist) {
                        ntfs_log_debug("Couldn't decompress the data runs.\n");
                }

                file->max_size = max(file->max_size, data->size_data);
                file->max_size = max(file->max_size, data->size_init);

                list_add_tail(&data->list, &file->data);
                count++;
        }

        ntfs_attr_put_search_ctx(ctx);
        ntfs_log_debug("File has %d data streams.\n", count);
        return count;
}

/**
 * free_file - Release the resources used by a file object
 * @file:  The unwanted file object
 *
 * This will free up the memory used by a file object and iterate through the
 * object's children, freeing their resources too.
 *
 * Return:  none
 */
void NtfsPlugin::free_file(struct ufile *file)
{
        struct list_head *item, *tmp;

        if (!file)
                return;

        list_for_each_safe(item, tmp, &file->name) { /* List of filenames */
                struct filename *f = list_entry(item, struct filename, list);
                ntfs_log_debug("freeing filename '%s'", f->name ? f->name :
                                NONE);
                if (f->name)
                        free(f->name);
                if (f->parent_name) {
                        ntfs_log_debug(" and parent filename '%s'",
                                        f->parent_name);
                        free(f->parent_name);
                }
                ntfs_log_debug(".\n");
                free(f);
        }

        list_for_each_safe(item, tmp, &file->data) { /* List of data streams */
                struct data *d = list_entry(item, struct data, list);
                ntfs_log_debug("Freeing data stream '%s'.\n", d->name ?
                                d->name : UNNAMED);
                if (d->name)
                        free(d->name);
                if (d->runlist)
                        free(d->runlist);
                free(d);
        }

        free(file->mft);
        free(file);
}

/**
 * read_record - Read an MFT record into memory
 * @vol:     An ntfs volume obtained from ntfs_mount
 * @record:  The record number to read
 *
 * Read the specified MFT record and gather as much information about it as
 * possible.
 *
 * Return:  Pointer  A ufile object containing the results
 *	    NULL     Error
 */
struct ufile* NtfsPlugin::read_record(ntfs_volume *vol, long long record, QStringList &names)
{
        ATTR_RECORD *attr10, *attr20, *attr90;
        struct ufile *file;
        ntfs_attr *mft;

        if (!vol)
                return NULL;

        file = (ufile*)calloc(1, sizeof(*file));
        if (!file) {
                ntfs_log_error("ERROR: Couldn't allocate memory in read_record()\n");
                return NULL;
        }

        INIT_LIST_HEAD(&file->name);
        INIT_LIST_HEAD(&file->data);
        file->inode = record;

        file->mft = (MFT_RECORD*)malloc(vol->mft_record_size);
        if (!file->mft) {
                ntfs_log_error("ERROR: Couldn't allocate memory in read_record()\n");
                free_file(file);
                return NULL;
        }

        mft = ntfs_attr_open(vol->mft_ni, AT_DATA, AT_UNNAMED, 0);
        if (!mft) {
                ntfs_log_perror("ERROR: Couldn't open $MFT/$DATA");
                free_file(file);
                return NULL;
        }

        if (ntfs_attr_mst_pread(mft, vol->mft_record_size * record, 1, vol->mft_record_size, file->mft) < 1) {
                ntfs_log_error("ERROR: Couldn't read MFT Record %lld.\n", record);
                ntfs_attr_close(mft);
                free_file(file);
                return NULL;
        }

        file->mft_lcn = mft->rl[0].lcn;
        file->mft_len = mft->rl[0].length;

        ntfs_attr_close(mft);
        mft = NULL;

        attr10 = find_first_attribute(AT_STANDARD_INFORMATION,	file->mft);
        attr20 = find_first_attribute(AT_ATTRIBUTE_LIST,	file->mft);
        attr90 = find_first_attribute(AT_INDEX_ROOT,		file->mft);

        ntfs_log_debug("Attributes present: %s %s %s.\n", attr10?"0x10":"",
                        attr20?"0x20":"", attr90?"0x90":"");

        if (attr10) {
                STANDARD_INFORMATION *si;
                si = (STANDARD_INFORMATION *) ((char *) attr10 + le16_to_cpu(attr10->value_offset));
                file->date = ntfs2utc(si->last_data_change_time);
        }

        if (attr20 || !attr10)
                file->attr_list = 1;
        if (attr90)
                file->directory = 1;

        if (get_filenames(file, vol, names) < 0) {
                ntfs_log_error("ERROR: Couldn't get filenames.\n");
        }
        if (get_data(file, vol) < 0) {
                ntfs_log_error("ERROR: Couldn't get data streams.\n");
        }

        return file;
}


/**
 * undelete_file - Recover a deleted file from an NTFS volume
 * @vol:    An ntfs volume obtained from ntfs_mount
 * @inode:  MFT Record number to be recovered
 *
 * Read an MFT Record and try an recover any data associated with it.  Some of
 * the clusters may be in use; these will be filled with zeros or the fill byte
 * supplied in the options.
 *
 * Each data stream will be recovered and saved to a file.  The file's name will
 * be the original filename and it will be written to the current directory.
 * Any named data stream will be saved as filename:streamname.
 *
 * The output file's name and location can be altered by using the command line
 * options.
 *
 * N.B.  We cannot tell if someone has overwritten some of the data since the
 *       file was deleted.
 *
 * Return:  0  Error, something went wrong
 *	    1  Success, the data was recovered
 */
void NtfsPlugin::clusters_file(ntfs_volume *vol, long long inode)
{
        struct ufile *file;
        int i, j;
        long long start, end;
        runlist_element *rl;
        struct list_head *item;
        long long k;
        QStringList names;

        if (!vol)
                return;

        /* try to get record */
        file = read_record(vol, inode, names);
        myfile *f = new myfile;
        f->full_name = names.join("/").mid(1);
        if (!file || !file->mft) {
                ntfs_log_error("Can't read info from mft record %lld.\n", inode);
                return;
        }

        if (list_empty(&file->data)) {
                goto free;
        }

        list_for_each(item, &file->data) {
                struct data *d = list_entry(item, struct data, list);
                if (d->resident) {
                    m_mftRecords[inode] = f;
                } else {
                        rl = d->runlist;
                        if (!rl) {
                                ntfs_log_verbose("File has no runlist, hence no data.\n");
                                continue;
                        }

                        if (rl[0].length <= 0) {
                                ntfs_log_verbose("File has an empty runlist, hence no data.\n");
                                continue;
                        }

                        if (rl[0].lcn == LCN_RL_NOT_MAPPED) {	/* extended mft record */
                                ntfs_log_verbose("Missing segment at beginning, %lld "
                                                "clusters.\n",
                                                (long long)rl[0].length);
                                for (k = 0; k < rl[0].length * vol->cluster_size; k += vol->cluster_size) {
                                }
                        }

                        for (i = 0; rl[i].length > 0; i++) {
                                if (rl[i].lcn == LCN_RL_NOT_MAPPED)
                                        continue;

                                if (rl[i].lcn == LCN_HOLE)
                                        continue;

                                start = rl[i].lcn;
                                end   = rl[i].lcn + rl[i].length;

                                if (start < 0)
                                    continue;

                                for (j = start; j < end; j++)
                                    m_clusters[j] = f;
                        }
                }
        }
free:
        free_file(file);
}

/**
 * scan_disk - Search an NTFS volume for files that could be undeleted
 * @vol:  An ntfs volume obtained from ntfs_mount
 *
 * Read through all the MFT entries looking for deleted files.  For each one
 * determine how much of the data lies in unused disk space.
 *
 * The list can be filtered by name, size and date, using command line options.
 *
 * Return:  -1  Error, something went wrong
 *	     n  Success, the number of recoverable files
 */
int NtfsPlugin::scan_disk(ntfs_volume *vol)
{
        s64 nr_mft_records;
        const int BUFSIZE = 8192;
        char *buffer = NULL;
        int results = 0;
        ntfs_attr *attr;
        long long size;
        long long bmpsize;
        int i, j, k, b;

        if (!vol)
                return -1;

        attr = ntfs_attr_open(vol->mft_ni, AT_BITMAP, AT_UNNAMED, 0);
        if (!attr) {
                ntfs_log_perror("ERROR: Couldn't open $MFT/$BITMAP");
                return -1;
        }
        bmpsize = attr->initialized_size;

        buffer = (char *)malloc(BUFSIZE);
        if (!buffer) {
                ntfs_log_error("ERROR: Couldn't allocate memory in scan_disk()\n");
                results = -1;
                goto out;
        }

        nr_mft_records = vol->mft_na->initialized_size >>
                        vol->mft_record_size_bits;

        for (i = 0; i < bmpsize; i += BUFSIZE) {
                long long read_count = min((bmpsize - i), BUFSIZE);
                size = ntfs_attr_pread(attr, i, read_count, buffer);
                if (size < 0)
                        break;

                for (j = 0; j < size; j++) {
                        b = buffer[j];
                        for (k = 0; k < 8; k++, b>>=1) {
                                if (((i+j)*8+k) >= nr_mft_records)
                                        goto out;

                                clusters_file(vol, (i+j)*8+k);
                        }
                }
        }
out:
        free(buffer);
        if (attr)
                ntfs_attr_close(attr);
        return results;
}

QString NtfsPlugin::getFilename(quint64 offset)
{
    s64 cluster = offset / m_vol->cluster_size;
    myfile *f;

    // if non-resident stream
    if (cluster > m_vol->mft_zone_end)
    {
        if (cluster > m_len)
            return QString();

        f = m_clusters[cluster];
    }

    // if resident stream
    else
    {
        cluster = offset / m_vol->mft_record_size - m_vol->mft_zone_pos * m_mftRecordsInCluster;

        if (cluster > m_mftRecordsNum)
            return QString();

        f = m_mftRecords[cluster];
    }

    if (f)
        return f->full_name;

    else
        return QString();
}

int NtfsPlugin::prepare(const QString &tmpPath, const QString &device)
{
    ntfs_log_set_handler(ntfs_log_handler_outerr);

    utils_set_locale();

    m_vol = utils_mount_volume(device.toUtf8().constData(), (ntfs_mount_flags)(NTFS_MNT_RDONLY | NTFS_MNT_FORCE));
    if (!m_vol)
        return 0;

    m_mftRecordsInCluster = m_vol->cluster_size / m_vol->mft_record_size;    
    m_mftRecordsNum = m_mftRecordsInCluster * m_vol->mft_zone_end;

    m_len = m_vol->nr_clusters * sizeof(myfile*);
    m_tmpFilename = QDir(tmpPath).absoluteFilePath(device.mid(device.lastIndexOf('/') + 1));

    m_mftLen = m_mftRecordsNum * sizeof(myfile*);
    m_tmpMftFilename = QDir(tmpPath).absoluteFilePath(device.mid(device.lastIndexOf('/') + 1) + "mft");

    if (!filePrepare(m_tmpFilename, m_len, &m_fd))
        return 0;

    if (!map((void**)&m_clusters, m_len, m_fd))
        return 0;

    if (!filePrepare(m_tmpMftFilename, m_mftLen, &m_mftFd))
        return 0;

    if (!map((void**)&m_mftRecords, m_mftLen, m_mftFd))
        return 0;

    memset(m_clusters, 0, m_len);
    memset(m_mftRecords, 0, m_mftLen);

    if (scan_disk(m_vol))
    {
        perror("ERROR: Failed to scan device");
        return 0;
    }

    return 1;
}

int NtfsPlugin::finalize()
{
    ntfs_umount(m_vol, (BOOL)FALSE);

    fileFinalyze(m_clusters, m_len, m_fd);
    fileFinalyze(m_mftRecords, m_mftLen, m_mftFd);

    if (!QFile::remove(m_tmpFilename))
        perror("Error removing temporary file");

    if (!QFile::remove(m_tmpMftFilename))
        perror("Error removing temporary file");

    return 0;
}

bool NtfsPlugin::fsmatch(const QString &type)
{
    QStringList list = QStringList() << "ntfs";

    return list.contains(type, Qt::CaseInsensitive);
}

Q_EXPORT_PLUGIN2(NtfsPlugin, NtfsPlugin);
