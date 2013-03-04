#ifndef DICTEDITWINDOW_H
#define DICTEDITWINDOW_H

#include <QDialog>
#include <QProcess>
#include <QMessageBox>
#include "maingui.h"
#include "ui_dicteditwindow.h"

class MainGui ;

class DictEditWindow : public QDialog, public Ui::DictEditWindow
{
    Q_OBJECT

public:
    explicit DictEditWindow(MainGui *mw, QWidget *parent = 0);
    ~DictEditWindow();

private slots:
    void on_cbxDictionary_activated(int index);
    void on_pushButton_clicked();

private:
    Ui::DictEditWindow *ui;
    MainGui *mainWindow ;
    QProcess *proc ;

    void fillDictList(const QVector<QFileInfo>* dicts) ;
    bool fill_wordsList(const QVector<QFileInfo>* dicts, int i) ;
    bool runTextEditor(QStringList &dirList, const QString &progName) ;
};

#endif // DICTEDITWINDOW_H
