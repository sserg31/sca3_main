#ifndef FSINTERFACE_H
#define FSINTERFACE_H

#include <QString>
#include <QFile>
#include <QtPlugin>

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

class BaseFSParser
{
public:
    virtual ~BaseFSParser() {}

    /// returns true plugin can handle filesystem type
    virtual bool fsmatch(const QString &type) = 0;

    /// returns filename by offset from beginning
    virtual QString getFilename(quint64 offset) = 0;

    /// reads system info structures
    virtual int prepare(const QString &tmpPath, const QString &device) = 0;

    /// closes some handles in necessary
    virtual int finalize() = 0;

protected:
    QString m_tmpFilename;
    int m_fd;
    size_t m_len;

    bool filePrepare(QString filename, size_t len, int *fd)
    {
        *fd = open(filename.toUtf8().constData(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        if (*fd == -1) {
            fprintf(stderr, "Error opening tmp file for writing");
            return false;
        }

        /* Stretch the file size to the size of the (mmapped) array of ints
         */
        if (lseek(*fd, len-1, SEEK_SET) == -1) {
            close(*fd);
            fprintf(stderr, "Error calling lseek() to 'stretch' the tmp file");
            return false;
        }

        if (write(*fd, "", 1) != 1) {
            close(*fd);
            fprintf(stderr, "Error writing last byte of the file");
            return false;
        }

        return true;
    }

    bool map(void **data, size_t len, int fd)
    {
        *data = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (*data == MAP_FAILED) {
            close(fd);
            fprintf(stderr, "Error mmapping the file");
            return false;
        }

        return true;
    }

    void fileFinalyze(void *data, size_t len, int fd)
    {
        if (munmap(data, len) == -1)
            perror("Error un-mmapping the file");

        close(fd);
    }
};

Q_DECLARE_INTERFACE(BaseFSParser, "com.trolltech.Plugin.BaseFSParser/1.0");

#endif
