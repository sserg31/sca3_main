#ifndef MAINGUI_H
#define MAINGUI_H

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QMessageBox>
#include <QModelIndex>

#include <iostream>
using namespace std;

#include "ui_mainwindow.h"

#include "service_library.h"
#include "../diskfind_console/support.h"
#include "searchresultsdialog.h"
#include "dicteditwindow.h"

class SearchResultsDialog;

class MainGui: public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

    struct HighlightedItem
    {
        quint64 block;
        int offset;
        int len;
        bool needHighlight;
        QString device;
    };

    friend class DictEditWindow ;
public:
    MainGui();
    ~MainGui();

    void set_sector(QString device, int block, int offset, int len, bool needShowSector = true);

public slots:
    void switch_use_dict(bool);
    void switch_use_phrase(bool);
    void show_disk_size(int);
    void show_sector();
    void update_coding();
    void show_dict_words(int);
    void search_data();

protected:
    void closeEvent(QCloseEvent* e);

signals:
    void aboutToClose();

private:
    QVector<QFileInfo>* load_dicts(const QString& path);
    QStringList load_dict_words(const QString& dictfile);
    QVector<Device> findDevices();

    #if QT_VERSION < 0x040500
    int removeDuplicates(QStringList &list);
    #endif

    HighlightedItem m_highlightedItem;
    static const QColor m_highlightColor;

    void fillDevList(const QVector<Device>  &m_devs);
    bool fillDictList(const QVector<QFileInfo>* dicts);
    bool fill_wordsList(const QVector<QFileInfo>* dicts, int i);
    bool fillHexTable(const QByteArray& bytes);

    QString kbytes_string(quint64 kbytes);
    QString convert_to_unicode(QString word, QTextCodec* codec);
    QString to_pretty_string(const QByteArray& str);

    QString format_cmd_string(const QString &path,
                              const QString &dev,
                              const QStringList &searchdicts,
                              const QString &words,
                              const QStringList &enc,
                              const QStringList &types,
                              bool analyseFilesystem,
                              const QString &progressIPC);

    QStringList wordsFromDicts(const QStringList &searchdicts);

    QVector<Device> m_devs;
    QVector<QFileInfo>* dicts;

    QMap<QString,QString> st_table;

    unsigned int m_currentTaskNum;
    QTimer *m_updateDevTimer;
    static const int m_updateDevInterval = 2000;

private slots:
    void updateDevList();
    void on_editDictButton_clicked();
    void on_addDictButton_clicked();
};

#endif // MAINGUI_H
