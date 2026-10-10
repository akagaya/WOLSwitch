#ifndef WOL_H
#define WOL_H

#include <QByteArray>
#include <QString>

#include <optional>

namespace wol {

// Wake-on-LAN の送信先 UDP ポート
inline constexpr quint16 Port = 7;

// "HH:HH:HH:HH:HH:HH" (区切りは ':' または '-') を 6 バイトに変換する。
// 形式が不正な場合は std::nullopt を返す。
std::optional<QByteArray> parseMac(const QString &text);

// マジックパケット (0xFF x 6 + MAC x 16 = 102 バイト) を生成する。
// mac は parseMac() が返した 6 バイトであること。
QByteArray magicPacket(const QByteArray &mac);

} // namespace wol

#endif // WOL_H
