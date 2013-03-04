#ifndef DISKFIND_H
#define DISKFIND_H

#include <QStringList>
#include <QFile>
#include <QVector>
#include <QMap>
#include <QTextCodec>
#include <QDateTime>

#include <QXmlStreamWriter>

#include <stdlib.h>
#include <iostream>
#include <ctype.h>

#include <typed_parser.h>

#include <support.h>

#include <signature.h>

using namespace std;

QT_BEGIN_NAMESPACE
class QMutex;
QT_END_NAMESPACE

typedef QMap<QString, QList<QByteArray>*> MultiDict;

class Diskfind : public QObject
{
public:
    enum OutputFormat
    {
        Xml     = 0x00000001,
        Stdout  = 0x00000002
    };

    typedef QFlags<OutputFormat> OutputFormats;
    typedef QMap<OutputFormat, QString> OutputFiles;

    struct SearchParams
    {
        QString device;
        QStringList dictionaries;
        QStringList words;
        QStringList encodings;
        QStringList types;
        QString cmdLine;
        OutputFiles files;
        QString progressIPC;
    };

    static Diskfind* instance();
    virtual ~Diskfind();

    void scanDevice(const QString &dev,
                     const QStringList &dict,
                     const QStringList &enc,
                     const QStringList &types,
                     const QString &cmdline,
                     const QString &progressIPC);

    void setOutputFiles(const OutputFiles &files);
    void setTermFlag(bool state);

private:
    Diskfind();

    void parseConfig();

    void detectFSType(const QString &dev);

    void print_results(QVector<SearchResult*>* v);

    QVector<SearchResult*>* scan_typed_files(QVector<TypedFile*>* files, MultiDict dict);

    QVector<SearchResult*>* scan_dict_buf(QByteArray* buf,
                                          MultiDict dict,
                                          long long unsigned block,
                                          const QString& type,
                                          long long unsigned offset);

    MultiDict* create_multi_coding_dict(const QStringList& dict,
                                        const QStringList& enc);


    int scan_word_buffer(QByteArray* buf,
                         const QByteArray& word, int from);

    bool only_latin(const QString& word);

    int concat_vectors(QVector<SearchResult*>* res, QVector<SearchResult*>* op);
    int erase_pointer_vector(QVector<SearchResult*>* vec);

    static Diskfind *m_instance;

    QMap<QString, QTextCodec*> codecs;
    PluginsLoader* m_plugins;
    BaseFSParser *m_fsParser;
    char m_stdoutSep;

    void beginReports();
    void endReports();
    bool m_reportStarted;

    OutputFormats m_formats;
    OutputFiles m_outputFiles;

    QFile m_xmlFile;
    QXmlStreamWriter m_xmlWriter;

    QDateTime m_beginScanTime;
    QDateTime m_endScanTime;

    QString m_dev, m_cmdLine;
    QStringList m_dict, m_enc, m_types;

    QMutex *m_termMutex;
    bool m_termFlag;

    bool getTermFlag();

    static QString m_configPath;
    static QString m_tmpStoreName;
    static QString m_defaultTmpStorePath;
    QString m_tmpStorePath;

protected:
    void timerEvent(QTimerEvent *);

};

#endif // DISKFIND_H
