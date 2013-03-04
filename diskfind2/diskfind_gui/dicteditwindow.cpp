#include "dicteditwindow.h"

DictEditWindow::DictEditWindow(MainGui *mw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DictEditWindow)
{
    ui->setupUi(this);
    mainWindow = mw ;
    proc = new QProcess ;

    fillDictList(mw->dicts) ;

    this->exec() ;
}

DictEditWindow::~DictEditWindow()
{
    delete ui;
    delete proc ;
}


void DictEditWindow::on_cbxDictionary_activated(int index)
{
    ui->wordsList->clear();

    if (fill_wordsList(mainWindow->dicts, index))
        ui->curDictName->setText(ui->cbxDictionary->itemText(index));
    else
        ui->curDictName->clear();
}

void DictEditWindow::on_pushButton_clicked()
{
    bool isInList=false ;
    QStringList dirList ;
    for (int i=0; i < ui->cbxDictionary->count(); i++)
    {
        isInList = ui->cbxDictionary->itemData(i, Qt::CheckStateRole).toBool();
        if (isInList)
        {
            dirList << ui->cbxDictionary->itemData(i).toString();
        }
    }

    if (dirList.count()==0)
    {
        QMessageBox::information(0, "diskFind", trUtf8("Нет выбранных словарей")) ;
        return ;
    }
    //run external program(kate or gedit)
    bool checkRun;
    if (!(checkRun = runTextEditor(dirList, "kate")))
    {
        checkRun = runTextEditor(dirList, "gedit") ;
    }

    if(!checkRun)
    {
        //ни один текстовый редактор не запустился
        QMessageBox::information(0, "Error", trUtf8("Не удалось запустить ни Kate ни Gedit.\nВозможно данные программы не установлены"));
    }
    //хоть один текстовый редактор(kate или gedit) запутился и отработал
}


bool DictEditWindow::runTextEditor(QStringList &dirList, const QString &progName)
{
    proc->start(progName, dirList) ;
    if (!proc->waitForStarted(1000))
    {
        return false;
    }else
    {
        if (proc->waitForFinished(-1))
        {
            QString delFileName ;
            foreach(const QString &word, dirList)
            {
                delFileName = word + '~' ;
                QFile(delFileName).remove();

            }
            return true ;
        }
    }
    //недостижимо
    return false ;
}

void DictEditWindow::fillDictList(const QVector<QFileInfo>* dicts)
{
    if (dicts)
    {
        for (int i=0; i < dicts->size(); ++i)
        {
            ui->cbxDictionary->addItem((*dicts)[i].baseName(), (*dicts)[i].absoluteFilePath());
            ui->cbxDictionary->setItemData(i, false, Qt::CheckStateRole);
        }
    }
}


bool DictEditWindow::fill_wordsList(const QVector<QFileInfo>* dicts, int i)
{
    if (i==-1 || dicts->count() <= i)
        return false;

    QStringList words = mainWindow->load_dict_words((*dicts)[i].absoluteFilePath());

    foreach(const QString &word, words)
        ui->wordsList->addItem(word);

    return true;
}
