#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QUdpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_tableMACAddr_itemSelectionChanged();

    void on_tableMACAddr_cellClicked(int row, int column);

private:
    void initLocalIPv4Address();
    void readAddrList();
    bool writeAddrList();

    Ui::MainWindow *ui;

    QString listfile;

    QUdpSocket udpsocket;
};

#endif // MAINWINDOW_H
