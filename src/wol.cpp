#include "wol.h"

#include <QRegularExpression>

namespace wol {

std::optional<QByteArray> parseMac(const QString &text)
{
    static const QRegularExpression pattern(
        QStringLiteral("^[0-9A-Fa-f]{2}([:-][0-9A-Fa-f]{2}){5}$"));

    const QString trimmed = text.trimmed();
    if (!pattern.match(trimmed).hasMatch())
        return std::nullopt;

    QString hex = trimmed;
    hex.remove(QLatin1Char(':')).remove(QLatin1Char('-'));
    return QByteArray::fromHex(hex.toLatin1());
}

QByteArray magicPacket(const QByteArray &mac)
{
    Q_ASSERT(mac.size() == 6);

    QByteArray packet(6, '\xFF');
    packet.reserve(6 + 16 * mac.size());
    for (int i = 0; i < 16; ++i)
        packet.append(mac);
    return packet;
}

} // namespace wol
