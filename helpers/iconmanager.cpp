#include "iconmanager.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

IconManager::IconManager()
{
    icon_dir = new QDir("icons/");
}

QStringList IconManager::getIconList()
{
    icon_dir->refresh();

    QStringList extensions;
    extensions << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";
    QFileInfoList icon_paths = icon_dir->entryInfoList(extensions, QDir::Files);

    QStringList validated_icon_paths;
    for (QFileInfo icon_path : icon_paths) {
        if (icon_path.permission(QFileDevice::ReadOther)) {
            validated_icon_paths.append(icon_path.filePath());
            qDebug() << icon_path.filePath();
        }
    }

    return validated_icon_paths;
}
