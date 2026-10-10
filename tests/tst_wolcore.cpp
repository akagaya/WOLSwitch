#include "hostlist.h"
#include "wol.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

class TestWolCore : public QObject
{
    Q_OBJECT

private slots:
    void parseMac_data();
    void parseMac();
    void magicPacket();
    void magicPacketWithZeroByte();
    void hostListRoundTrip();
    void hostListLoadsV1Format();
    void hostListMissingFile();
};

void TestWolCore::parseMac_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QByteArray>("expected"); // 空 = 不正

    QTest::newRow("colon") << "02:1a:2b:3c:4d:5c" << QByteArray::fromHex("021a2b3c4d5c");
    QTest::newRow("upper") << "02:AB:CD:EF:12:34" << QByteArray::fromHex("02abcdef1234");
    QTest::newRow("hyphen") << "02-de-ad-be-ef-01" << QByteArray::fromHex("02deadbeef01");
    QTest::newRow("zero bytes") << "00:11:00:22:00:33" << QByteArray::fromHex("001100220033");
    QTest::newRow("empty mask") << ":::::" << QByteArray();
    QTest::newRow("incomplete") << "02:1a:2b:3c:4d:5" << QByteArray();
    QTest::newRow("too long") << "02:1a:2b:3c:4d:5c:00" << QByteArray();
    QTest::newRow("not hex") << "gg:1a:2b:3c:4d:5c" << QByteArray();
    QTest::newRow("no separator") << "021a2b3c4d5c" << QByteArray();
}

void TestWolCore::parseMac()
{
    QFETCH(QString, text);
    QFETCH(QByteArray, expected);

    const std::optional<QByteArray> mac = wol::parseMac(text);
    QCOMPARE(mac.has_value(), !expected.isEmpty());
    if (mac)
        QCOMPARE(*mac, expected);
}

void TestWolCore::magicPacket()
{
    const QByteArray mac = QByteArray::fromHex("021a2b3c4d5c");
    const QByteArray packet = wol::magicPacket(mac);

    QCOMPARE(packet.size(), 102);
    QCOMPARE(packet.left(6), QByteArray(6, '\xFF'));
    for (int i = 0; i < 16; ++i)
        QCOMPARE(packet.mid(6 + i * 6, 6), mac);
}

// v1.0 は MAC を C 文字列として連結していたため、0x00 を含むとパケットが欠けていた
void TestWolCore::magicPacketWithZeroByte()
{
    const QByteArray mac = QByteArray::fromHex("001100220033");
    const QByteArray packet = wol::magicPacket(mac);

    QCOMPARE(packet.size(), 102);
    for (int i = 0; i < 16; ++i)
        QCOMPARE(packet.mid(6 + i * 6, 6), mac);
}

void TestWolCore::hostListRoundTrip()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath("hosts.csv");

    const QList<Host> hosts = {
        {"サーバー", "02:de:ad:be:ef:01"},
        {"nas", "02:AB:CD:EF:12:34"},
    };
    QVERIFY(hostlist::save(path, hosts));
    QCOMPARE(hostlist::load(path), hosts);
}

void TestWolCore::hostListLoadsV1Format()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath("hosts.csv");

    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("pc1,02:1a:2b:3c:4d:5c\r\n\r\npc2,02:00:5e:10:20:30\r\n");
    file.close();

    const QList<Host> expected = {
        {"pc1", "02:1a:2b:3c:4d:5c"},
        {"pc2", "02:00:5e:10:20:30"},
    };
    QCOMPARE(hostlist::load(path), expected);
}

void TestWolCore::hostListMissingFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(hostlist::load(dir.filePath("missing.csv")).isEmpty());
}

QTEST_APPLESS_MAIN(TestWolCore)
#include "tst_wolcore.moc"
