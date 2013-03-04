
#include <qapplication.h>
#include "maingui.h"
#include <QTranslator>
#include <QLibraryInfo>

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    QString locale = QLocale::system().name();

    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_%1").arg(QLocale::system().name().left(2)), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator translator;
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    translator.load(QString("diskfind_") + locale, "/usr/share/diskfind");
    a.installTranslator(&translator);

    MainGui w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
