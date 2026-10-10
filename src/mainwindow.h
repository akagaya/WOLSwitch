#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hostlist.h"

#include <QMainWindow>
#include <QUdpSocket>

#include <memory>

class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void populateInterfaces();
    void populateHostTable(const QList<Host> &hosts);
    QList<Host> hostsInTable() const;
    bool hasHostNamed(const QString &name) const;
    void appendHostRow(const Host &host);

    void onInterfaceChanged(int index);
    void onHostSelectionChanged();
    void onDeleteClicked();
    void onSendClicked();

    std::unique_ptr<Ui::MainWindow> ui;
    QLabel *m_ipLabel;
    QString m_listFile;
    QUdpSocket m_socket;
};

#endif // MAINWINDOW_H
