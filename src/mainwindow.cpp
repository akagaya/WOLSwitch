#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "wol.h"

#include <QCoreApplication>
#include <QDir>
#include <QFontDatabase>
#include <QHostAddress>
#include <QLabel>
#include <QNetworkInterface>
#include <QStandardPaths>

namespace {

enum Column { NameColumn, MacColumn };

QString defaultListFile()
{
    const QString fileName = QStringLiteral("WOLSwitch.csv");
#ifdef Q_OS_MACOS
    // .app バンドル内には書き込めないため、書類フォルダに置く
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
        .filePath(fileName);
#else
    return QDir(QCoreApplication::applicationDirPath()).filePath(fileName);
#endif
}

// ネットマスクが取得できない場合は /24 とみなす
QString broadcastFor(const QNetworkAddressEntry &entry)
{
    if (!entry.broadcast().isNull())
        return entry.broadcast().toString();

    const QString ip = entry.ip().toString();
    return ip.left(ip.lastIndexOf(QLatin1Char('.')) + 1) + QStringLiteral("255");
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , m_ipLabel(new QLabel(this))
    , m_listFile(defaultListFile())
{
    ui->setupUi(this);
    ui->statusBar->addWidget(m_ipLabel);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->hostTable->setFont(fixedFont);
    ui->macEdit->setFont(fixedFont);
    ui->broadcastEdit->setFont(fixedFont);

    connect(ui->interfaceCombo, &QComboBox::currentIndexChanged,
            this, &MainWindow::onInterfaceChanged);
    connect(ui->hostTable, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onHostSelectionChanged);
    connect(ui->sendButton, &QPushButton::clicked,
            this, &MainWindow::onSendClicked);

    populateInterfaces();
    populateHostTable(hostlist::load(m_listFile));
    if (ui->hostTable->rowCount() > 0)
        ui->hostTable->selectRow(0);
}

MainWindow::~MainWindow() = default;

void MainWindow::populateInterfaces()
{
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (!iface.flags().testFlag(QNetworkInterface::IsUp))
            continue;

        const QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            const QHostAddress ip = entry.ip();
            if (ip.protocol() != QAbstractSocket::IPv4Protocol || ip.isLoopback())
                continue;
            ui->interfaceCombo->addItem(ip.toString(), broadcastFor(entry));
        }
    }
}

void MainWindow::populateHostTable(const QList<Host> &hosts)
{
    ui->hostTable->setRowCount(0);
    for (const Host &host : hosts)
        appendHostRow(host);
}

QList<Host> MainWindow::hostsInTable() const
{
    QList<Host> hosts;
    for (int row = 0; row < ui->hostTable->rowCount(); ++row) {
        hosts.append({ui->hostTable->item(row, NameColumn)->text(),
                      ui->hostTable->item(row, MacColumn)->text()});
    }
    return hosts;
}

bool MainWindow::hasHostNamed(const QString &name) const
{
    for (int row = 0; row < ui->hostTable->rowCount(); ++row) {
        if (ui->hostTable->item(row, NameColumn)->text() == name)
            return true;
    }
    return false;
}

void MainWindow::appendHostRow(const Host &host)
{
    const int row = ui->hostTable->rowCount();
    ui->hostTable->insertRow(row);
    ui->hostTable->setItem(row, NameColumn, new QTableWidgetItem(host.name));
    ui->hostTable->setItem(row, MacColumn, new QTableWidgetItem(host.mac));
}

void MainWindow::onInterfaceChanged(int index)
{
    if (index < 0)
        return;

    ui->broadcastEdit->setText(ui->interfaceCombo->itemData(index).toString());
    m_ipLabel->setText(tr("IP:%1").arg(ui->interfaceCombo->itemText(index)));
}

void MainWindow::onHostSelectionChanged()
{
    const int row = ui->hostTable->currentRow();
    if (row < 0)
        return;

    ui->nameEdit->setText(ui->hostTable->item(row, NameColumn)->text());
    ui->macEdit->setText(ui->hostTable->item(row, MacColumn)->text());
}

void MainWindow::onSendClicked()
{
    const int messageTimeoutMs = 5000;

    const QString macText = ui->macEdit->text();
    const std::optional<QByteArray> mac = wol::parseMac(macText);
    if (!mac) {
        ui->statusBar->showMessage(tr("MACアドレスが不正です"), messageTimeoutMs);
        return;
    }

    // 未登録の名前であれば一覧に追加する
    const QString name = ui->nameEdit->text().trimmed();
    if (!name.isEmpty() && !hasHostNamed(name))
        appendHostRow({name, macText});
    const bool saved = hostlist::save(m_listFile, hostsInTable());

    const QHostAddress broadcast(ui->broadcastEdit->text().trimmed());
    if (broadcast.isNull()) {
        ui->statusBar->showMessage(tr("ブロードキャストアドレスが不正です"), messageTimeoutMs);
        return;
    }

    const QByteArray packet = wol::magicPacket(*mac);
    if (m_socket.writeDatagram(packet, broadcast, wol::Port) != packet.size()) {
        ui->statusBar->showMessage(tr("送信に失敗しました: %1").arg(m_socket.errorString()),
                                   messageTimeoutMs);
        return;
    }

    ui->statusBar->showMessage(saved ? tr("送信しました: %1").arg(macText)
                                     : tr("送信しましたが、一覧の保存に失敗しました"),
                               messageTimeoutMs);
}
