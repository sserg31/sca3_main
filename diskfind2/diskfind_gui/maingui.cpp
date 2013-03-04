 #include "maingui.h"

#include "markeritemdelegate.h"
#include "itemdataroles.h"

#define BLOCKSIZE 512

const QColor MainGui::m_highlightColor = Qt::yellow;

MainGui::MainGui()
{
    m_currentTaskNum = 0;

    m_highlightedItem.needHighlight = false;
    m_highlightedItem.len = 0;
    m_highlightedItem.offset = 0;
    m_highlightedItem.block = 0;

    setupUi(this);

    connect(chxUseDictionary,SIGNAL(toggled(bool)),this, SLOT(switch_use_dict(bool)));
    connect(chxUsePhrase,SIGNAL(toggled(bool)),this, SLOT(switch_use_phrase(bool)));
    connect(devSelectCombo,SIGNAL(activated(int)),this, SLOT(show_disk_size(int)));
    connect(devSelectCombo,SIGNAL(activated(int)),this, SLOT(show_sector()));
    connect(cbxDictionary,SIGNAL(activated(int)),this, SLOT(show_dict_words(int)));
    connect(showSectorButton, SIGNAL(clicked()),this,SLOT(show_sector()));
    connect(toggleSearchButton,SIGNAL(clicked()),this,SLOT(search_data()));
    connect(searchTextEdit,SIGNAL(returnPressed()),this,SLOT(search_data()));
    connect(dataCoding,SIGNAL(currentIndexChanged(int)),this,SLOT(update_coding()));
    connect(sectorNumberSpin, SIGNAL(valueChanged(int)), this, SLOT(show_sector()));

    QAction *quitAction = new QAction(this);
    quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    addAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    QFont f = QFont();
    f.setFixedPitch(true);
    hexTable->setFont(QFont("Monospace"));
    hexTable->setItemDelegateForColumn(hexTable->columnCount()-1, new MarkerItemDelegate);
    updateDevList();
    if (devSelectCombo->count())
    {
        devSelectCombo->setCurrentIndex(0);
        show_disk_size(0);
    }

    dicts = load_dicts(QApplication::applicationDirPath() + "/../share/diskfind/dicts");
    fillDictList(dicts);

    cbxEncodings->addItem("UTF-8", "utf8");
    cbxEncodings->addItem("UNIX (KOI8-R)", "koi8r");
    cbxEncodings->addItem("WINDOWS (CP1251)", "cp1251");
    cbxEncodings->addItem("DOS (CP866)", "cp866");
    cbxEncodings->addItem(tr("All encodings"), "all");

    for(int i = 0; i < cbxEncodings->count(); i++)
        cbxEncodings->setItemData(i, false, Qt::CheckStateRole);

    cbxEncodings->setItemData(cbxEncodings->findData("all"), true, Qt::CheckStateRole);

    cbxDocTypes->addItem("Microsoft Office", "doc");
    cbxDocTypes->addItem("Microsoft Office Open XML", "docx");
    cbxDocTypes->addItem("OpenDocument", "odt");
    cbxDocTypes->addItem("PDF", "pdf");
    cbxDocTypes->addItem(tr("All types"), "all");

    cbxDocTypes->setItemData(0, "*.doc *.xls", Qt::ToolTipRole);
    cbxDocTypes->setItemData(1, "*.docx *.xlsx", Qt::ToolTipRole);
    cbxDocTypes->setItemData(2, "*.odt *.ods *.odg *.odp", Qt::ToolTipRole);
    cbxDocTypes->setItemData(3, "*.pdf", Qt::ToolTipRole);

    for(int i = 0; i < cbxDocTypes->count(); i++)
        cbxDocTypes->setItemData(i, false, Qt::CheckStateRole);

    cbxDocTypes->setItemData(cbxDocTypes->findData("all"), true, Qt::CheckStateRole);

    dataCoding->addItem("ASCII", "");
    dataCoding->addItem("DOS (CP866)", "cp866");
    dataCoding->addItem("ISO-8859-5", "ISO-8859-5");
    dataCoding->addItem("UNIX (KOI8-R)", "koi8r");
    dataCoding->addItem("WINDOWS (CP1251)", "cp1251");

    wordsList->setEnabled(true);
    switch_use_dict(false);
    switch_use_phrase(false);

    m_updateDevTimer = new QTimer;
    connect(m_updateDevTimer, SIGNAL(timeout()), this, SLOT(updateDevList()));
    m_updateDevTimer->start(m_updateDevInterval);

    hexTable->update();
    hexTable->horizontalHeader()->setUpdatesEnabled(false);
//    for (int i = 0; i < hexTable->columnCount()-1; i++)
//        hexTable->horizontalHeader()->resizeSection(i, 35);
    hexTable->horizontalHeader()->setUpdatesEnabled(true);
    hexTable->horizontalHeader()->setStretchLastSection(true);

    setWindowIcon(QIcon(":/share/pixmaps/diskfind2.xpm"));
}

void MainGui::updateDevList()
{
    m_devs = findDevices();
    fillDevList(m_devs);
}

MainGui::~MainGui()
{
    if (dicts)
        delete dicts;
}

void MainGui::set_sector(QString device, int block, int offset, int len, bool needShowSector)
{
    int index = -1;
    int i = 0;

    foreach (const Device &d, m_devs)
    {
        if (d.name == device)
        {
            index = i;
            break;
        }

        i++;
    }

    if (index != -1)
        devSelectCombo->setCurrentIndex(index);

    else
        return;

    m_highlightedItem.needHighlight = true;
    m_highlightedItem.device = device;
    m_highlightedItem.block = block;
    m_highlightedItem.offset = offset;
    m_highlightedItem.len = len;

    int row, column;

    if (needShowSector)
    {
        sectorNumberSpin->blockSignals(true);
        show_disk_size(index);
        show_sector();
        sectorNumberSpin->blockSignals(false);
        sectorNumberSpin->setValue(block);
    }

    int c = hexTable->columnCount();
    int r = hexTable->rowCount();

    const int columns = 0x10;

    row =  (offset & (~(columns-1))) / columns;
    column = offset & (columns-1);

    if (len)
        hexTable->scrollToItem(hexTable->item(row, column));

    int lastRow = row;
    int beginHighlight = column;
    bool notNullLen = len;

    while (len && (row < r))
    {
        // + 4 for empty columns
        hexTable->item(row, column + (column / 4))->setData(Qt::BackgroundRole, m_highlightColor);
        hexTable->item(row, c-1)->setData(ItemDataRoles::HighLightColorRole, m_highlightColor);

        row += (column + 1) / columns;

        if (lastRow != row)
        {
            hexTable->item(lastRow, c-1)->setData(ItemDataRoles::BeginHighLightRole, beginHighlight);
            hexTable->item(lastRow, c-1)->setData(ItemDataRoles::EndHighLightRole, column+1);

            lastRow = row;
            beginHighlight = 0;
        }

        column = (column + 1) % columns;
        len--;
    }

    if (notNullLen)
    {
        hexTable->item(lastRow, c-1)->setData(ItemDataRoles::BeginHighLightRole, beginHighlight);
        hexTable->item(lastRow, c-1)->setData(ItemDataRoles::EndHighLightRole, column);
    }
}

QVector<QFileInfo>* MainGui::load_dicts(const QString& path)
{
    QVector<QFileInfo>* dicts = new QVector<QFileInfo>;
    QDir dictDir(path);
    foreach( QString fileName, dictDir.entryList(QDir::Files))
    {
        dicts->append(QFileInfo(dictDir.absoluteFilePath(fileName)));
    }
    if (!dicts->size())
    {
        delete dicts;
        dicts = 0;
    }
    return dicts;
}

QVector<Device> MainGui::findDevices()
{
    QVector<Device> devs;
    QFile part("/proc/partitions");
    if (!part.open(QIODevice::ReadOnly))
        return devs;

    QString data = part.readAll();
    part.close();

    foreach (const QString pline, data.split("\n", QString::SkipEmptyParts))
    {
        QStringList slist = pline.simplified().split(QRegExp("[ \t]"));
        Device d_tmp;
        if (slist.count() == 4 && slist[2].toInt() > 0)
        {
            d_tmp.size = slist[2].toInt();
            d_tmp.name = slist[3];
            d_tmp.fullname = "/dev/" + d_tmp.name;

            system_functions::detectDevice(d_tmp);

            devs.append(d_tmp);
        }
    }

    return devs;
}

void MainGui::fillDevList(const QVector<Device> &devs)
{
    QStringList devsStr;

    foreach(const Device &dev, devs)
    {
        QString str = QString("%1: %2").arg(dev.type).arg(dev.name);
        if (!dev.fstype.isNull())
            str.append(QString(" (%1)").arg(dev.fstype));

        devsStr.append(str);
    }

    devSelectCombo->setDevs(devsStr);

    bool found = false;
    foreach (const Device &d, m_devs)
    {
        if (d.name == m_highlightedItem.device)
        {
            found = true;
            break;
        }
    }

    if (!found)
        m_highlightedItem.needHighlight = false;
}

bool MainGui::fillDictList(const QVector<QFileInfo>* dicts)
{
    if (!dicts)
        return false;

    for (int i=0; i < dicts->size(); ++i)
    {
        cbxDictionary->addItem((*dicts)[i].baseName(), (*dicts)[i].absoluteFilePath());
        cbxDictionary->setItemData(i, false, Qt::CheckStateRole);
    }

    if (dicts->size())
    {
        cbxDictionary->addItem(tr("All dictionaries"), "all");
        cbxDictionary->setItemData(cbxDictionary->count(), false, Qt::CheckStateRole);
    }

    return true;
}

bool MainGui::fill_wordsList(const QVector<QFileInfo>* dicts, int i)
{
    if (i==-1 || dicts->count() <= i)
        return false;

    QStringList words = load_dict_words((*dicts)[i].absoluteFilePath());

    foreach(const QString &word, words)
        wordsList->addItem(word);

    return true;
}

bool MainGui::fillHexTable(const QByteArray& bytes)
{
    int row=0;
    int column=0;
    int c=0;
    int DATAC = 20;
    QString coding = dataCoding->itemData(dataCoding->currentIndex()).toString();
    qDebug() << "Coding:  " << coding;
    QTextCodec* codec = get_codec(coding);

    for (int i=0; i< bytes.size(); ++i)
    {
        QString num = QString::number(bytes[i],16);
        if (num.length()==1)
            num.push_front("0");
        if(num.length()>2)
            num=num.right(2);
        QTableWidgetItem* n = new QTableWidgetItem(num);
        n->setFont(QFont("Monospace",10,QFont::Light));
        hexTable->setItem(row, column, n);
        column++;
        c++;
        if (column == 19){
            hexTable->setItem(row,column, new QTableWidgetItem("  ")); 
            c=0;
            column = 0;
            QTableWidgetItem* n;
            if (coding != "")
            {
                //n = new QTableWidgetItem(codec->toUnicode(to_pretty_string(bytes.mid(row*16,16)).toAscii()));
                //n = new QTableWidgetItem(codec->toUnicode(to_pretty_string(bytes.mid(row*16,16)).toLocal8Bit()));
                QByteArray arr = bytes.mid(row*16,16);
                for (int i=0; i< arr.size(); ++i)
                {
                    if (QChar(arr[i])<=32 && QChar(arr[i])>=0)
                    {
                     arr[i]='.';
                    }
                }
                cout<<"\n";
                n = new QTableWidgetItem(codec->toUnicode(QString(arr).toAscii()));
            }else{
               n = new QTableWidgetItem(to_pretty_string(bytes.mid(row*16,16)));
           }
            n->setFont(QFont("Monospace",10,QFont::Light));
            hexTable->setItem(row,DATAC, n);
            row++;
            continue;
        }
        if ( c == 4)
        {
            hexTable->setItem(row,column, new QTableWidgetItem("  "));
            c=0;
            column++;
        }
    }
    //hexTable->reset();
    //hexTable->horizontalHeader()->setUpdatesEnabled(false);
    //for (unsigned int i = 0; i < hexTable->columnCount()-1; i++)
    //    hexTable->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
    //hexTable->horizontalHeader()->setUpdatesEnabled(true);
    hexTable -> resizeColumnsToContents();
    hexTable -> resizeRowsToContents ();
    hexTable->horizontalHeader()->setStretchLastSection(true);
    this->activateWindow();
    hexTable->activateWindow();

    return true;
}

void MainGui::switch_use_dict(bool use)
{
    cbxDictionary->setEnabled(use);
    wordsList->clear();
    dictLabel->clear();

    if (use)
    {
        dictContentsWidget->show();
        dictLine->show();
        fill_wordsList(dicts, cbxDictionary->currentIndex());
    }

    else
    {
        dictContentsWidget->hide();
        dictLine->hide();
    }
}

void MainGui::update_coding()
{
    show_sector();
}

void MainGui::show_dict_words(int i)
{
    wordsList->clear();
    if (fill_wordsList(this->dicts, i))
        dictLabel->setText(cbxDictionary->itemText(i));
    else
        dictLabel->clear();
}

void MainGui::switch_use_phrase(bool use)
{
    searchTextEdit->setEnabled(use);
}

void MainGui::show_disk_size(int i)
{
    if (m_devs.isEmpty() || i < 0)
        return;

    const Device &dev = m_devs[i];
    qint64 s = dev.size * 1024LL / BLOCKSIZE;
    QString blocks;

    if (QLocale::system().name().left(2).toLower() == "ru")
    {
        int lastDigit = s % 10;

        if (lastDigit == 1)
            blocks = QString::fromUtf8("блок");

        else if (lastDigit > 1 && lastDigit < 5)
            blocks = QString::fromUtf8("блока");

        else
            blocks = QString::fromUtf8("блоков");
    }

    else
    {
        if (s == 1)
            blocks = tr("block");

        else
            blocks = tr("blocks");
    }

    devSizeLabel->setText(kbytes_string(dev.size) + QString(" (%1 %2)").arg(s).arg(blocks));

    cout << "dev size " << dev.size << endl;
    sectorNumberSpin->setMaximum(s-1);
    sectorNumberSpin->setValue(0);

    analyseFS->setEnabled(!dev.fstype.isEmpty());
}

void MainGui::show_sector()
{
    int devI = devSelectCombo->currentIndex();
    if (devI == -1)
        return;

    quint64 block = sectorNumberSpin->value();
    Device device = m_devs[devI];
    QByteArray bytes = system_functions::read_block(device.fullname, block, BLOCKSIZE);

    if (!bytes.size())
        QMessageBox::critical(this, tr("Error"), tr("Unable to open device"));

    fillHexTable(bytes);

    if (m_highlightedItem.needHighlight && m_highlightedItem.device == device.name && m_highlightedItem.block == block)
        set_sector(m_highlightedItem.device, m_highlightedItem.block, m_highlightedItem.offset, m_highlightedItem.len, false);
}

void MainGui::search_data()
{
    m_highlightedItem.needHighlight = false;

    QString phrase;
    QStringList dicts;
    QStringList enc;
    QStringList filetypes;
    Device dev;
    if (chxUsePhrase->checkState())
        phrase = searchTextEdit->text();

    int all_pos;
    bool all;
    if (chxUseDictionary->checkState())
    {
        all_pos = cbxDictionary->findData("all");
        all = cbxDictionary->itemData(all_pos, Qt::CheckStateRole).toBool();

        for( int i=0 ; i < cbxDictionary->count(); ++i)
        {
            if (all_pos == i)
                continue;

            bool e = cbxDictionary->itemData(i, Qt::CheckStateRole).toBool();
            if (e || all){
                dicts << cbxDictionary->itemData(i).toString();
            }
        }
    }
    //Add encodings
    all_pos = cbxEncodings->findData("all");
    all = cbxEncodings->itemData(all_pos, Qt::CheckStateRole).toBool();
    for( int i=0 ; i < cbxEncodings->count(); ++i)
    {
        if (all_pos == i)
            continue;

        bool e = cbxEncodings->itemData(i, Qt::CheckStateRole).toBool();
        if (e || all){
            enc << cbxEncodings->itemData(i).toString();
        }
    }

    if (enc.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Select encoding"));
        return;
    }

    all_pos = cbxDocTypes->findData("all");
    all = cbxDocTypes->itemData(all_pos, Qt::CheckStateRole).toBool();
    for( int i=0 ; i < cbxDocTypes->count(); ++i)
    {
        if (all_pos == i)
            continue;

        bool e = cbxDocTypes->itemData(i, Qt::CheckStateRole).toBool();
        if (e || all){
            filetypes << cbxDocTypes->itemData(i).toString();
        }
    }

    dev = m_devs[devSelectCombo->currentIndex()];

    QString path = QDir(QApplication::applicationDirPath()).absoluteFilePath("diskfind_console");
    QString sharedMemName = QString("diskfind_%1_%2").arg(getpid()).arg(m_currentTaskNum++);

    QString cmd = format_cmd_string(path, dev.fullname, dicts, phrase, enc, filetypes, analyseFS->isChecked() && analyseFS->isEnabled(), sharedMemName);
    if (cmd.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Select dictionary or phrase"));
        return;
    }

    SearchResultsDialog *searchDialog = new SearchResultsDialog(sharedMemName, dev.name, this);

    connect(this, SIGNAL(aboutToClose()), searchDialog, SLOT(close()));
    connect(searchDialog, SIGNAL(finished(int)), searchDialog, SLOT(deleteLater()));
    connect(searchDialog, SIGNAL(rejected()), searchDialog, SLOT(deleteLater()));

    searchDialog->setEncodings(enc);
    searchDialog->setFiletypes(filetypes);

    QStringList lst = wordsFromDicts(QStringList(dicts));

    if (!phrase.isEmpty())
        lst.prepend(phrase);

    searchDialog->setSearchWords(lst);
    cmd += searchDialog->getReportsArgs();
    cout << "CMD: " << cmd.toUtf8().constData() << endl;
    searchDialog->run(cmd);
}

QString MainGui::format_cmd_string(const QString &path,
                                   const QString &dev,
                                   const QStringList &searchdicts,
                                   const QString &word,
                                   const QStringList &enc,
                                   const QStringList &types,
                                   bool analyseFilesystem,
                                   const QString &progressIPC)
{
    QString base =  path + " -q --print0 ";

    if (analyseFilesystem)
        base += "-l ";

    base += "-i " + dev;

    if (searchdicts.isEmpty() && word.isEmpty())
        return "";

    foreach(const QString &dict, searchdicts)
        base += " -d \"" + dict + "\"";

    if (!word.isEmpty())
        base += " -w \"" + word + "\"";

    QString enc_str = " -e " + enc.join(" -e ");
    QString type_str;

    if (!types.isEmpty())
        type_str += " -t " + types.join(" -t ");

    QString end = base + enc_str + type_str;

    if (!progressIPC.isEmpty())
        end += " -p " + progressIPC;

    return end;
}

QStringList MainGui::load_dict_words(const QString& dictfile)
{
    QStringList dict;
    QFile f(dictfile);
    if (!f.open(QIODevice::ReadOnly))
        return dict;

    while (!f.atEnd()) {
        QString line = QString::fromUtf8(f.readLine(1024).trimmed());
        if (line != "")
            dict.push_back(line);
    }

    f.close();
    return dict;
}

QString MainGui::kbytes_string(quint64 kbytes)
{
    quint64 KB = 1LL;
    quint64 MB = 1024LL * KB;
    quint64 GB = 1024LL * MB;
    quint64 TB = 1024LL * GB;

    double tmp = kbytes;

    if (kbytes > TB)
        return QString::number(tmp/TB,' ',2) + tr(" TB");
    if (kbytes > GB)
        return QString::number(tmp/GB,' ',2) + tr(" GB");
    if (kbytes > MB)
        return QString::number(tmp/MB,' ',2) + tr(" MB");

    return QString::number(tmp,' ',2) + tr(" KB");
}

QString MainGui::convert_to_unicode(QString word, QTextCodec* codec)
{
    if (!codec)
    {
        exit(-1);
    }
    return codec->toUnicode(word.toAscii());
}

QString MainGui::to_pretty_string(const QByteArray& str)
{
    QString res;
    for(int i=0; i<str.size(); ++i)
    {
        if (str[i]<32)
        {
            res.push_back(0x2E);
        }else res.push_back(str[i]);
    }
    return res;
}

void MainGui::closeEvent(QCloseEvent* e)
{
    //emit aboutToClose();
    //cout << "Close main window" << endl;

    QMessageBox* pmbx = new QMessageBox("DiskFind",
                                        trUtf8("Вы уверены, что хотите закрыть окно?"),
                                        QMessageBox::Information,
                                        QMessageBox::Yes,
                                        QMessageBox::No,
                                        QMessageBox::Cancel | QMessageBox::Escape);
    int answer = pmbx->exec();
    delete pmbx;

    if (answer == QMessageBox::Yes)
    {
        emit aboutToClose();
        cout << "Close main window" << endl;
    }

    else
    {
        e->ignore();

    }
}

QStringList MainGui::wordsFromDicts(const QStringList &searchdicts)
{
    QStringList result;

    foreach(const QString &dict, searchdicts)
        result += load_dict_words(dict);

    #if QT_VERSION >= 0x040500
    result.removeDuplicates();
    #else
    removeDuplicates(result);
    #endif

    return result;
}

#if QT_VERSION < 0x040500
int MainGui::removeDuplicates(QStringList &list)
{
    int n = list.size();
    int j = 0;
    QSet<QString> seen;
    seen.reserve(n);
    for (int i = 0; i < n; ++i) {
        const QString &s = list.at(i);
        if (seen.contains(s))
            continue;
        seen.insert(s);
        if (j != i)
            list[j] = s;
        ++j;
    }
    if (n != j)
        list.erase(list.begin() + j, list.end());
    return n - j;
}
#endif

void MainGui::on_editDictButton_clicked()
{
    DictEditWindow *dictEdit = new DictEditWindow(this) ;
}

void MainGui::on_addDictButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home",
        tr("All Files (*);;Text Files (*.txt)"));

    if (fileName != "") {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"),
                tr("Could not open file"));
            return;
        }
        file.close();

        dicts->append(QFileInfo(fileName));

        //ставим предпоследним в checkBoxList, в списке словарей(dicts)будет последним
        cbxDictionary->insertItem(cbxDictionary->count()-1, (*dicts)[dicts->size()-1].baseName(), (*dicts)[dicts->size()-1].absoluteFilePath());
        cbxDictionary->setItemData(cbxDictionary->count()-1, false, Qt::CheckStateRole);

    }
}
