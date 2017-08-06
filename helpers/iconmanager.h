#ifndef ICONMANAGER_H
#define ICONMANAGER_H
#include <QStringList>

class QDir;

class IconManager
{
public:
    IconManager();
    QStringList getIconList();

private:
    QDir* icon_dir;
};

#endif // ICONMANAGER_H
