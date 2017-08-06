#include "views/mainwindow.h"
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //load custom fonts
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-MediumItalic.ttf") < 0)
        qDebug() << "Error while loading Roboto-MediumItalic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Medium.ttf") < 0)
        qDebug() << "Error while loading Roboto-Medium.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-BlackItalic.ttf") < 0)
        qDebug() << "Error while loading Roboto-BlackItalic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Black.ttf") < 0)
        qDebug() << "Error while loading Roboto-Black.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-LightItalic.ttf") < 0)
        qDebug() << "Error while loading Roboto-LightItalic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Light.ttf") < 0)
        qDebug() << "Error while loading Roboto-Light.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-ThinItalic.ttf") < 0)
        qDebug() << "Error while loading Roboto-ThinItalic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Thin.ttf") < 0)
        qDebug() << "Error while loading Roboto-Thin.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-BoldItalic.ttf") < 0)
        qDebug() << "Error while loading Roboto-BoldItalic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Bold.ttf") < 0)
        qDebug() << "Error while loading Roboto-Bold.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Italic.ttf") < 0)
        qDebug() << "Error while loading Roboto-Italic.ttf";
    if (QFontDatabase::addApplicationFont(":/Fonts/Resources/Fonts/Roboto/Roboto-Regular.ttf") < 0)
        qDebug() << "Error while loading Roboto-Regular.ttf";
    else
        QApplication::setFont(QFont("Roboto Regular", 10, QFont::Normal));

    //center the window
    w.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            w.size(),
            qApp->desktop()->availableGeometry()
        )
    );
    w.show();

    return a.exec();
}
