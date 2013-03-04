#ifndef SERVICE_LIBRARY_H
#define SERVICE_LIBRARY_H

#include <QString>

struct Device
{
    QString name;
    QString fullname;
    QString type;
    QString fstype;
    int size;
};

namespace system_functions
{
    enum DriveType
    {
        dtUNKNOWN_DEVICE    = 2,
        dtREMOVABLE         = 0x10,
        dtPARTITION         = 0x20,
        dtEXTENDEDPARTITION = 0x40,
        dtMOUNTEDIMAGE      = 0x80
    };

    unsigned int getDeviceType(QString path);
    QString getFilesystemType(const QString &path, bool *isExtended);
    QByteArray read_block(const QString &dev, quint64 block,int block_size);

    void detectDevice(Device &device);
}

#endif // SERVICE_LIBRARY_H
