#ifndef SEARCHRESULTSDIALOG_H
#define SEARCHRESULTSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDateTime>

#include "ui_searchresultdialog.h"
#include "../diskfind_console/support.h"

#include <iostream>

#include "maingui.h"
class MainGui;

using namespace std;
class SearchResultsDialog: public QDialog, public Ui::SearchResultsDialog
{
    Q_OBJECT
public:
    SearchResultsDialog(const QString &sharedMemName, const QString &dev, QWidget* parent = 0);
    ~SearchResultsDialog();

    void setEncodings(const QStringList &encodings) { m_encodings = encodings; }
    void setFiletypes(const QStringList &filetypes) { m_filetypes = filetypes; }
    void setSearchWords(const QStringList &searchwords) { m_searchwords = searchwords; }
    void connects();

    QVector<SearchResult*> convert_to_searchresult(const QByteArray& output);

    void run(const QString &cmd);

    QString getReportsArgs();

private:
    struct ReportConfig
    {
        QString postFix; // oX, X -- postfix in this case
        QString saveDglDescr;
        QString extension;

        ReportConfig(const QString &postfix, const QString &savedlgdecr, const QString &ext)
        {
            postFix = postfix;
            saveDglDescr = savedlgdecr;
            extension = ext;
        }
    };

    int numbWarningRow ;
    const static QByteArray m_newLineSep;

    void clean();
    void updateSearchResults(const QByteArray &data);
    void updateWindowTitle();
    bool isNeedSerachContinue() ;

protected:
    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *e);

public slots:
    void cellclicked(int row);
    void save_report();
    void cancelClicked();
    void pauseClicked();
    void reject();

    void erase();
    void rescan();

private:
    QWidget* m_maingui;
    QString m_device;
    QStringList m_encodings;
    QStringList m_filetypes;
    QStringList m_searchwords;

    QDateTime m_begin, m_end;

    QString m_searchCommand;
    QProcess *m_search;
    bool m_isPaused;
    QSharedMemory *m_sharedMem;
    QString m_sharedMemName;
    float m_percent;
    bool *m_isWindowDestroyed;

    QAction *m_quitAction;

    int m_timerId;
    bool m_canceled;

    static QDir m_tmpDir;

    static QList<ReportConfig> m_reportConfigs;

    QString getStoredFilename(const QString &ext);
};

#endif // SEARCHRESULTSDIALOG_H
