#ifndef HOSTLIST_H
#define HOSTLIST_H

#include <QList>
#include <QString>

struct Host
{
    QString name;
    QString mac;

    bool operator==(const Host &other) const
    {
        return name == other.name && mac == other.mac;
    }
};

// "名前,MACアドレス" 形式の CSV を読み書きする (v1.0 と同じ形式)。
namespace hostlist {

// ファイルが存在しない・開けない場合は空のリストを返す。
QList<Host> load(const QString &path);

bool save(const QString &path, const QList<Host> &hosts);

} // namespace hostlist

#endif // HOSTLIST_H
