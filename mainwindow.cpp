#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardPaths>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    initLocalIPv4Address();
    ui->statusBar->showMessage("IP:" + ui->comboBox->currentText());

    listfile = "WOLSwitch.csv";
#ifdef Q_OS_MAC
    listfile = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/WOLSwitch.csv";
    qDebug() << listfile;
#endif

    readAddrList();
    ui->tableMACAddr->selectRow(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initLocalIPv4Address(){
    QList<QHostAddress> rl = QNetworkInterface::allAddresses();
    QList<QHostAddress>::iterator it = rl.begin();

    while(it != rl.end()){
        qDebug() << it->toString();
        if(it->protocol() == QAbstractSocket::IPv4Protocol && !it->isLoopback()){
            ui->comboBox->addItem(it->toString());
            //break;
        }
        it++;
    }
}

void MainWindow::readAddrList(){
    QFile file(listfile);
    QString rowdata;
    QTableWidgetItem *cellitem;
    int i = 0;

    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        while(!in.atEnd()){
            ui->tableMACAddr->setRowCount(i + 1);

            rowdata = in.readLine();
            cellitem = new QTableWidgetItem(rowdata.left(rowdata.indexOf(",")));
            ui->tableMACAddr->setItem(i, 0, cellitem);

            cellitem = new QTableWidgetItem(rowdata.mid(rowdata.lastIndexOf(",") + 1));
            ui->tableMACAddr->setItem(i, 1, cellitem);

            i++;
        }
    }
    file.close();
}

bool MainWindow::writeAddrList(){
    QFile file(listfile);

    if(file.open(QIODevice::WriteOnly)){
        QTableWidget *tbl = ui->tableMACAddr;
        int rc = tbl->rowCount();
        QTextStream out(&file);
        for(int i = 0; i < rc; i++){
            out << tbl->item(i, 0)->text() << "," << tbl->item(i, 1)->text() << endl;
        }
    }else{
        return false;
    }
    file.close();
    return true;
}


void MainWindow::on_pushButton_clicked()
{
    if(ui->txtSendMac->text().length() != 17){
        return;
    }

    QString mac = ui->txtSendMac->text();
    QString name = ui->txtSendName->text();
    if(name != "" && ui->tableMACAddr->findItems(name, Qt::MatchEndsWith).count() == 0){
        int setrow = ui->tableMACAddr->rowCount();
        ui->tableMACAddr->setRowCount(setrow + 1);
        QTableWidgetItem *cellitem = new QTableWidgetItem(name);
        ui->tableMACAddr->setItem(setrow, 0, cellitem);
        cellitem = new QTableWidgetItem(mac);
        ui->tableMACAddr->setItem(setrow, 1, cellitem);
    }
    writeAddrList();

    int tmp = 0;
    char sendmac[7];
    for(int i = 0; i < 6; i++){
        tmp = ("0x" + mac.left(2)).toInt(0, 16);
        sendmac[i] = tmp;
        mac.remove(0, 3);
    }
    sendmac[6] = '\0';

    QByteArray sdata;
    for(int i = 0; i < 6; i++){
        sdata.append(0xFF);
    }
    for(int i = 0; i < 16; i++){
        sdata.append(sendmac);
    }

    QHostAddress * saddr = new QHostAddress(ui->thxtBcastAddr->text());
    udpsocket.writeDatagram(sdata, *saddr, 7);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->thxtBcastAddr->setText(arg1.left(arg1.lastIndexOf(".") + 1) + "255");
    ui->statusBar->showMessage("IP:" + arg1);

}

void MainWindow::on_tableMACAddr_itemSelectionChanged()
{
    int cnt = ui->tableMACAddr->currentRow();

    ui->txtSendName->setText(ui->tableMACAddr->item(cnt, 0)->text());
    ui->txtSendMac->setText(ui->tableMACAddr->item(cnt, 1)->text());
}

void MainWindow::on_tableMACAddr_cellClicked(int row, int column)
{
    int cnt = ui->tableMACAddr->currentRow();

    ui->txtSendName->setText(ui->tableMACAddr->item(cnt, 0)->text());
    ui->txtSendMac->setText(ui->tableMACAddr->item(cnt, 1)->text());
}
