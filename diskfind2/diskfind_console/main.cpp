#include <QCoreApplication>
#include <QtCore>

#include <stdlib.h>
#include <iostream>
#include <locale.h>

using namespace std;

#include <QStringList>
#include <QTextCodec>
#include <QTextStream>

#include "diskfind.h"

#include <getopt.h>

#ifndef Q_OS_WIN32
#include <signal.h>
#endif

bool print0 = 0;
bool analyseFS = 0;
bool quiet = 0;

void usage(char *name)
{
    printf("usage: %s\n\
       \t-h \t\t this message\n\
       \t-i <device> \t device to scan [for example: -i /dev/sda]\n\
       \t-q  \t\t output only search resuts to stdout\n\
       \t-l  \t\t analyse filesystem for filenames\n\
       \t-d <file> \t dictionary with words for searching [for example: -d dic1 -d dic2]\n\
       \t-w  \t\t word list in quotes for searching\n\
       \t-e <encoding> \t encodings to analyze [for example: -e utf8 -e koi8r]\n\
       \t-t <filetype> \t filetype to scan [for example: -t doc -t pdf]\n\
       \t-oX <file> \t output scan in xml to the given filename\n\
       \t--help \t\t this message\n\
       \t--print0 \t print record fields separated by null character and double null character instead of colon and newline symbol respectively\n\
       \t--stdout \t output to console\n", name);
}

QTextCodec* get_codec()
{
    QString lc = QString(locale("").name().c_str());
    if (lc == "C")
    {
        cout<<"Please set your locale"<<endl;
        return 0;
    }
    int i = lc.indexOf('.');
    if (i == -1)
    {
        return QTextCodec::codecForName(lc.toAscii());
    }else{
        return QTextCodec::codecForName(lc.right(lc.length() - i -1).toAscii());
    }
    return 0;

}

QStringList convert_to_unicode(const QStringList words)
{
    QTextCodec* codec = get_codec();
    if (!codec)
    {
        exit(-1);
    }

    QStringList wordsUni;
    foreach (const QString &word, words)
    {
        wordsUni.push_back(codec->toUnicode(word.toUtf8()));
    }

    return wordsUni;
}

QStringList load_dict(const QStringList& dictfiles)
{
    QStringList dict;
    for (int i=0; i < dictfiles.size(); ++i)
    {
        QFile f(dictfiles[i]);
        if (!f.open(QIODevice::ReadOnly))
            continue;

        while (!f.atEnd()) {
            QString line = QString::fromUtf8(f.readLine(1024)).trimmed();
            if (line != "")
                dict.push_back(line);
        }
    }
    return dict;
}

void runtask(Diskfind *d, Diskfind::SearchParams &params)
{
    d->setOutputFiles(params.files);

    QStringList l = load_dict(params.dictionaries);

    #if QT_VERSION >= 0x040500
    params.words += l;
    #else
    for (int i = 0; i < l.size(); ++i)
    {
        params.words.append(l[i]);
    }
    #endif
    d->startTimer(100);

    QStringList w = convert_to_unicode(params.words);
    d->scanDevice(params.device, w, params.encodings, params.types, params.cmdLine, params.progressIPC);
}

volatile sig_atomic_t handler_in_progress = 0;

void termination_handler(int )
{
    if (handler_in_progress)
        ;

    else
    {
        handler_in_progress = 1;

        Diskfind::instance()->setTermFlag(true);
    }
}

int main(int argc, char *argv[])
{
    // setting signal handler to finish xml
    signal(SIGINT, termination_handler);
    signal(SIGTERM, termination_handler);
    signal(SIGQUIT, termination_handler);

    QCoreApplication app(argc, argv);

    if(argc == 1)
    {
        usage(argv[0]);
        exit(-1);
    }

    Diskfind::SearchParams params;

    QString cmdLine;
    int rez = 0;

    for(int i = 0; i < argc; i++)
        cmdLine += argv[i] + QString(" ");

    params.cmdLine = cmdLine.trimmed();

    QString param;

    int optIdx;
    static struct option long_opt[] = {
                                         {"help", 0, 0, 'h'},
                                         {"stdout", 0, 0, 0},
                                         {"print0", 0, 0, 0},
                                         {"oX", 1, 0, 0},
                                         {0, 0, 0, 0}
                                      };
    while ((rez = getopt_long_only(argc, argv, "d:w:t:i:e:p:hlq", long_opt, &optIdx)) != EOF)
    {
        switch (rez)
        {
            case 0:     param = long_opt[optIdx].name;
                        if (param == "stdout")
                            params.files.insert(Diskfind::Stdout, "");

                        else if (param == "oX")
                            params.files.insert(Diskfind::Xml, optarg);

                        else if (param == "print0")
                            print0 = true;
                        break;

            case 'l':   analyseFS = true;
                        break;

            case 'q':   quiet = true;
                        break;

            case 'd':   params.dictionaries.push_back(optarg);
                        break;

            case 'w':   params.words.push_back(QString::fromUtf8(optarg));
                        break;

            case 't':   params.types.push_back(optarg);
                        break;

            case 'i':   params.device = optarg;
                        break;

            case 'e':   params.encodings.push_back(optarg);
                        break;

            case 'p':   params.progressIPC = QString(optarg);
                        break;

            case '?': case 'h': default:
                        usage(argv[0]);
                        return -1;
                        break;
        }
    }

    if (params.device.isEmpty())
    {
        puts("device not set");
        return -1;
    }

    if (params.words.isEmpty() && params.dictionaries.isEmpty())
    {
        puts("neither dicts nor word lists for search are set");
        return -1;
    }

    Diskfind *d = Diskfind::instance();
    QtConcurrent::run(runtask, d, params);

    return app.exec();
}
