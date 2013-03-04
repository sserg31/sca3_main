#include "service_library.h"

//sys internals
#include <locale.h>

#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>

#include <QRegExp>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>

namespace system_functions
{
    unsigned int getDeviceType(QString path)
    {
        unsigned int res = 0;
        using namespace std;

        QString tmp;
        char in;
        QRegExp rx("(\\d+)"); //searching digits
        if(rx.indexIn(path) != -1)
            res = dtPARTITION;

        path.replace(rx, QString(""));

        if (!QDir("/sys/block/" + path).exists(QString("%1").arg(path)+"1"))
            res = dtPARTITION;

        tmp = "/sys/block/" + path.toLocal8Bit() + "/removable";
        int dFile = open(tmp.toLocal8Bit(), O_NONBLOCK|O_RDONLY);
        ///qDebug()<<"checking result";
        if(dFile > 0)
        {
            read(dFile, &in, 1);
            //qDebug()<< "1 bytes have read";
            if(in == '1') // chech if it's removable
            {
                res = res | dtREMOVABLE;
            }
            close(dFile);
        }
        else
        {
            if (path != "loop")
                res = dtUNKNOWN_DEVICE;
            else
                res |= dtMOUNTEDIMAGE;
        }
        return res;
    }

    QString getFilesystemType(const QString &path, bool *isExtended)
    {
        QProcess process;
        process.start("disktype " + path);
        process.waitForFinished();

        QStringList lines = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts);
        int pos;
        QString fstype;
        foreach(const QString &str, lines)
        {
            pos = str.indexOf("file system");
            if (pos != -1)
            {
                fstype = str.left(pos).trimmed().toLower();
                break;
            }

            if (str.indexOf("Type") != -1 && str.indexOf("Extended") != -1)
                *isExtended = true;
        }

        // disktype don't know ext4, we check it
        if (fstype == "ext3")
        {
            process.start("file -s " + path);
            process.waitForFinished();

            if (process.readAllStandardOutput().contains("ext4"))
                fstype = "ext4";
        }

        return fstype;
    }

    QByteArray read_block(const QString &dev, quint64 block, int block_size)
    {
        QFile f(dev);
        QByteArray bytes;

        if (!f.open(QIODevice::ReadOnly))
        {
            qDebug() << dev.toUtf8().constData();
            return bytes;
        }
        f.seek(block*block_size);
        QByteArray data = f.read(block_size);
        f.close();
        return data;
    }

    void detectDevice(Device &device)
    {
        unsigned int type = getDeviceType(device.name);

        if (type & dtPARTITION)
        {
            bool isExtended = false;

            device.fstype = getFilesystemType(device.fullname, &isExtended);

            if (isExtended)
                type = (type & ~dtPARTITION) | dtEXTENDEDPARTITION;
        }

        if (type & dtEXTENDEDPARTITION)
            device.type = QString(QObject::tr("Extended partition"));

        else if (type & dtREMOVABLE)
        {
            if (type & dtPARTITION)
                device.type = QString(QObject::tr("Logical disk"));

            else
                device.type = QString(QObject::tr("Removable device"));
        }

        else if (type &  dtPARTITION)
        {
            if (type & dtMOUNTEDIMAGE)
                device.type = QString(QObject::tr("Logical disk image"));

            else
                device.type = QString(QObject::tr("Logical disk"));
        }

        else
            device.type = QString(QObject::tr("Physical disk"));
    }
}
