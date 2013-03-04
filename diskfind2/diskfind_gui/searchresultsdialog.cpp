#include "searchresultsdialog.h"

#include <signal.h>

#define LIMIT 10000

QList<SearchResultsDialog::ReportConfig> SearchResultsDialog::m_reportConfigs = QList<ReportConfig>() <<
     ReportConfig("X", QObject::tr("Xml files (*.xml)"), ".xml") <<
     ReportConfig("", QObject::tr("Text files (*.txt)"), ".txt");

QDir SearchResultsDialog::m_tmpDir = QDir::temp();
const QByteArray SearchResultsDialog::m_newLineSep = QByteArray(2, '\0');

SearchResultsDialog::SearchResultsDialog(const QString &sharedMemName, const QString &dev, QWidget *parent)
    : QDialog()
{
    setupUi(this);

    m_isPaused = false;
    m_isWindowDestroyed = NULL;

    m_percent = 0.0f;
    m_canceled = false;
    m_search = new QProcess;
    m_device = dev;

    m_maingui = parent;

    m_sharedMemName = sharedMemName;

    resTable->sortItems(1);

    numbWarningRow = 250000; //при превышения примернотакого количества строк приложение занимает слишком много памяти, далее может повиснуть,
                              //надо спросить пользователя, не пора ли остановиться.

    connects();
    updateWindowTitle();
    setWindowIcon(QIcon(":/share/pixmaps/diskfind2.xpm"));

    show();
}

SearchResultsDialog::~SearchResultsDialog()
{
    delete m_quitAction;
    delete m_search;

    foreach (const ReportConfig &config, m_reportConfigs)
        QFile::remove(getStoredFilename(config.extension));

    if (m_isWindowDestroyed)
        *m_isWindowDestroyed = true;
}

void SearchResultsDialog::updateWindowTitle()
{
    setWindowTitle(tr("Searching in") + QString(" /dev/%1 - %2 %").arg(m_device).arg((int)m_percent));
}

void SearchResultsDialog::connects()
{
    m_quitAction = new QAction(this);
    m_quitAction->setShortcut(QKeySequence(tr("Ctrl+W")));
    addAction(m_quitAction);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_erase, SIGNAL(clicked()),this, SLOT(erase()));
    connect(saveReport, SIGNAL(clicked()),this, SLOT(save_report()));
    connect(m_cancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(m_pause, SIGNAL(clicked()), SLOT(pauseClicked()));
    connect(m_rescan, SIGNAL(clicked()), SLOT(rescan()));
    connect(resTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellclicked(int)));
}

void SearchResultsDialog::cellclicked(int row)
{
    QTableWidgetItem* tmp = resTable->item(row, 1);
    bool ok;
    int pos = tmp->text().toInt(&ok, 10);
    int offset = resTable->item(row, 2)->text().toInt(&ok, 16);
    int len = 0;

    QString buzztext = resTable->item(row, 0)->text();
    QString enc = resTable->item(row, 3)->text();

    QMap<QString, QTextCodec*> codecs;
    codecs["cp1251"] = QTextCodec::codecForName("CP1251");
    codecs["koi8r"] = QTextCodec::codecForName("KOI8-R");
    codecs["utf8"] = QTextCodec::codecForName("UTF-8");
    codecs["cp866"] = QTextCodec::codecForName("CP866");

    if (codecs.contains(enc))
        len = codecs[enc]->fromUnicode(buzztext).length();

    if (resTable->item(row, 4)->text().toLower() != "raw")
        len = 0;

    ((MainGui*)(m_maingui))->set_sector(m_device, pos, offset, len);
}

void SearchResultsDialog::save_report()
{
    QStringList list;
    foreach (const ReportConfig &config, m_reportConfigs)
        list.append(config.saveDglDescr);

    QString filterSel;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Saving report"),
                                                    QDir(getenv("HOME")).absoluteFilePath(QString("diskfind-%1-_dev_%2-%3%%4").arg(m_end.toString(Qt::ISODate)).arg(m_device).arg(m_percent).arg(m_reportConfigs.first().extension)),
                                                    list.join(";;"), &filterSel);
    if (fileName.isEmpty())
        return;

    int i = list.indexOf(filterSel);
    QString ext = m_reportConfigs[i].extension;

    if (!fileName.toLower().endsWith(ext))
        fileName += ext;

    if (ext != ".txt")
    {
        if (QFile::exists(fileName))
            QFile::remove(fileName);

        QFile::copy(getStoredFilename(ext), fileName);

        return;
    }

    cout << fileName.toStdString() << endl;
    QFile report(fileName);
    if (!report.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical((QWidget*)0, tr("Error"), report.errorString());
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QTextStream stream( &report );
    stream << tr("Created at: ") << now.toString("hh:mm:ss dd.MM.yyyy") << endl;
    stream << tr("Started at: ") << m_begin.toString("hh:mm:ss dd.MM.yyyy") << endl;
    stream << tr("Finished at: ") << m_end.toString("hh:mm:ss dd.MM.yyyy") << endl;
    stream << tr("Scan percent: ") << QString("%1%").arg(m_percent) << endl;
    stream << tr("Search at device: ") << "/dev/" + m_device << endl;
    stream << tr("Used encodings: ") << m_encodings.join(", ") << endl;
    stream << tr("Used filetypes plugins: ");

    if (m_filetypes.isEmpty())
        stream << tr("raw data search only") << endl;

    else
        stream << m_filetypes.join(", ") << ", " << tr("raw data search") << endl;

    const QString leftMargin = QString().fill(' ', 10);
    stream << tr("Search phrases: ") << endl << leftMargin << m_searchwords.join(QString("\n") + leftMargin) << endl;

    stream << endl << endl << tr("Search results:") << endl;

    stream  << qSetFieldWidth(15)
            << tr("Text")
            << tr("Block number")
            << tr("Offset")
            << tr("Encoding")
            << qSetFieldWidth(5)
            << tr("File type")
            << tr("File path") << endl;

    for (int i=0; i < this->resTable->rowCount() ; ++i)
    {
        stream  << qSetFieldWidth(15)
                << resTable->item(i, 0)->text()
                << resTable->item(i, 1)->text()
                << resTable->item(i, 2)->text()
                << resTable->item(i, 3)->text()
                << qSetFieldWidth(5)
                << resTable->item(i, 4)->text()
                << resTable->item(i, 5)->text() << endl;
    }

    report.close();
}

void SearchResultsDialog::erase()
{
    if(!QProcess::startDetached("shreg -x " + getStoredFilename(".xml")))
        QMessageBox::critical(this, tr("Error"), tr("Unable to launch shreg"));
}

void SearchResultsDialog::clean()
{
    killTimer(m_timerId);
    m_sharedMem->detach();

    delete m_sharedMem;
}

QVector<SearchResult*> SearchResultsDialog::convert_to_searchresult(const QByteArray& output)
{
    QList<QByteArray> l;
    int pos = 0, newPos = 0;
    while (newPos != -1)
    {
        newPos = output.indexOf(m_newLineSep, pos);

        if (newPos == -1)
            break;

        // if no name found
        if ((newPos + 2) < output.length() && output[newPos+2] == '\0')
            newPos++;

        l.append(output.mid(pos, newPos - pos));
        pos = newPos + 2;
    }

    QVector<SearchResult*> res;

    foreach (const QByteArray &arr, l)
    {
        QByteArray tmp = arr.trimmed();
        if (tmp == "")
            continue;

        QList<QByteArray> vals = tmp.split('\0');
        if (vals.size() < 5)
            continue;

        SearchResult* t = new SearchResult;
        t->str = QString::fromUtf8(vals[0]);
        t->block = vals[1].toLongLong();
        t->offset= vals[2].toLongLong();
        t->enc = vals[3];
        t->resulttype = vals[4];

        if (vals.count() > 5)
            t->filepath = QString::fromUtf8(vals[5]);

        res.append(t);
    }

    return res;
}

void SearchResultsDialog::timerEvent(QTimerEvent *)
{
    m_sharedMem->lock();
    memcpy(&m_percent, m_sharedMem->data(), sizeof(m_percent));
    m_sharedMem->unlock();

    m_progressBar->setValue(m_percent);

    updateSearchResults(m_search->readAllStandardOutput());
    updateWindowTitle();
}

void SearchResultsDialog::run(const QString &cmd)
{
    m_canceled = false;
    m_rescan->setVisible(false);
    m_cancel->setVisible(true);

    m_cancel->setEnabled(true);
    m_pause->setEnabled(true);

    saveReport->setEnabled(false);
    m_erase->setEnabled(false);

    resTable->clearContents();
    resTable->setRowCount(0);

    m_searchCommand = cmd;

    m_begin = QDateTime::currentDateTime();
    m_search->start(cmd);
    bool isWindowDestroyed = false;
    m_isWindowDestroyed = &isWindowDestroyed;


    m_sharedMem = new QSharedMemory(m_sharedMemName);
    if (!m_sharedMem->create(sizeof(m_percent)))
    {
        qDebug() << "Unable to create shared memory segment";
        qDebug() << "attach attempting";
        if (m_sharedMem->attach(QSharedMemory::ReadWrite))
            qDebug() << "Unable to attach shared memory segment";
    }

    if (m_search->waitForStarted(1000))
    {
        m_timerId = startTimer(100);

        while (!isWindowDestroyed && m_search->state() == QProcess::Running && !m_search->waitForFinished(100))
            qApp->processEvents();

        if (isWindowDestroyed)
            return;

        clean();

        m_end = QDateTime::currentDateTime();
        if (!m_canceled)
        {
            m_percent = 100.0f;
            m_progressBar->setValue(m_percent);
            updateWindowTitle();
        }

        updateSearchResults(m_search->readAllStandardOutput());
        m_erase->setEnabled(resTable->rowCount());

        resTable->setSortingEnabled(true);
        m_cancel->setEnabled(false);
        m_pause->setEnabled(false);
        saveReport->setEnabled(true);
    }

    else
    {
        m_cancel->setEnabled(false);
        m_pause->setEnabled(false);
        saveReport->setEnabled(false);

        QMessageBox::critical(this, tr("Error"), tr("Unable to launch diskfind_console.\nStart arguments: ") + cmd);
    }

    m_cancel->setVisible(false);
    m_rescan->setVisible(true);
    m_isWindowDestroyed = NULL;
}

void SearchResultsDialog::cancelClicked()
{
    if (m_canceled)
        return;

    if (m_isPaused)
        kill(m_search->pid(), SIGCONT);

    m_search->terminate();

    m_canceled = true;
}

void SearchResultsDialog::reject()
{
    cancelClicked();
    QDialog::reject();
}

void SearchResultsDialog::closeEvent(QCloseEvent *e)
{
    //cancelClicked();
    //emit finished(0);

    bool notWasPause = !m_isPaused ;
    if (!m_canceled && !m_isPaused)
        pauseClicked();

    QMessageBox* pmbx = new QMessageBox("DiskFind",
                                        trUtf8("Вы уверены, что хотите закрыть окно поиска?"),
                                        QMessageBox::Information,
                                        QMessageBox::Yes,
                                        QMessageBox::No,
                                        QMessageBox::Cancel | QMessageBox::Escape);

    int answer = pmbx->exec();
    delete pmbx;

    if (answer == QMessageBox::Yes)
    {
        cancelClicked();
        emit finished(0);
    }

    else
    {
        if (!m_canceled && notWasPause)
            pauseClicked();
        e->ignore();

    }
}

void SearchResultsDialog::updateSearchResults(const QByteArray &data)
{
    if (data.isEmpty())
        return;

    QVector<SearchResult*> res = convert_to_searchresult(data);

    int c = res.size() > LIMIT ? LIMIT : res.size();
    int i = resTable->rowCount();

    if (c+i > numbWarningRow){
        if (!isNeedSerachContinue()){
            //нажать стоп и выйти из функции
            cancelClicked() ;
            return ;
        }
    }

    resTable->setRowCount(i + c);

    resTable->setSortingEnabled(false);
    for (int j = 0; j < c; ++i, ++j)
    {
        QString str = res[j]->str;
        QTableWidgetItem* tmp = new QTableWidgetItem(str);
        tmp->setFlags(Qt::ItemIsEnabled);
        resTable->setItem(i, 0, tmp);

        tmp = new QTableWidgetItem;
        tmp->setFlags(Qt::ItemIsEnabled);
        tmp->setData(Qt::DisplayRole, res[j]->block);
        resTable->setItem(i, 1, tmp);

        tmp = new QTableWidgetItem(QString("0x%1").arg(QString::number(res[j]->offset, 16).toUpper(), 4, '0'));
        tmp->setFlags(Qt::ItemIsEnabled);
        resTable->setItem(i, 2, tmp);

        tmp = new QTableWidgetItem(res[j]->enc);
        tmp->setFlags(Qt::ItemIsEnabled);
        resTable->setItem(i, 3, tmp);


        tmp = new QTableWidgetItem(res[j]->resulttype);
        tmp->setFlags(Qt::ItemIsEnabled);
        resTable->setItem(i, 4, tmp);

        tmp = new QTableWidgetItem(res[j]->filepath);
        tmp->setFlags(Qt::ItemIsEnabled);
        resTable->setItem(i, 5, tmp);
    }
    resTable->setSortingEnabled(true);

    qDeleteAll(res);
}

QString SearchResultsDialog::getReportsArgs()
{
    QString res;
    foreach (const ReportConfig &config, m_reportConfigs)
    {
        if (!config.postFix.isEmpty())
            res += QString("-o%1 %2").arg(config.postFix).arg(getStoredFilename(config.extension));
    }

    return " " + res + " --stdout";
}

QString SearchResultsDialog::getStoredFilename(const QString &ext)
{
    return m_tmpDir.absoluteFilePath(m_sharedMemName+ext);
}

void SearchResultsDialog::pauseClicked()
{
    int signal = m_isPaused ? SIGCONT : SIGSTOP;

    if (kill(m_search->pid(), signal) == 0)
        m_isPaused = !m_isPaused;

    if (m_isPaused)
        m_pause->setText(tr("Resume"));

    else
        m_pause->setText(tr("Pause"));
}

void SearchResultsDialog::rescan()
{
    run(m_searchCommand);
}


bool SearchResultsDialog :: isNeedSerachContinue()
{
    pauseClicked();
    QMessageBox* pmbx = new QMessageBox("DiskFind",
                                        trUtf8("Приложение занимает слишком много памяти. При продолжении поиска возможно некорректное завершение."
                                               "\n Продолжать поиск?"),
                                        QMessageBox::Information,
                                        QMessageBox::Yes,
                                        QMessageBox::No,
                                        QMessageBox::Cancel | QMessageBox::Escape);

    int answer = pmbx->exec();
    delete pmbx;

    if (answer == QMessageBox::Yes)
    {
        pauseClicked();
        numbWarningRow += numbWarningRow;
        return true;
    }

    return false ;
}
