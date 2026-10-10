#include "hostlist.h"

#include <QFile>
#include <QSaveFile>
#include <QTextStream>

namespace hostlist {

QList<Host> load(const QString &path)
{
    QList<Host> hosts;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return hosts;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // 名前は最初のカンマまで、MAC アドレスは最後のカンマ以降
        const QString name = line.left(line.indexOf(QLatin1Char(','))).trimmed();
        const QString mac = line.mid(line.lastIndexOf(QLatin1Char(',')) + 1).trimmed();
        hosts.append({name, mac});
    }
    return hosts;
}

bool save(const QString &path, const QList<Host> &hosts)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    for (const Host &host : hosts)
        out << host.name << ',' << host.mac << '\n';
    out.flush();

    return file.commit();
}

} // namespace hostlist
