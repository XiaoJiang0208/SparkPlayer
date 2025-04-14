#include "sparkplayer.h"

//#include <QApplication>
#include <DApplication>
#include <DApplicationSettings>
#include <DWidgetUtil>
#include <DLog>
#include <DTitlebar>
#include <QLocale>
#include <QTranslator>
#include <DAboutDialog>
#include <QKeySequence>
#include "utils/qhotkey.h"

DWIDGET_USE_NAMESPACE
int main(int argc, char *argv[])
{
    DApplication *a = DApplication::globalApplication(argc, argv);
    Dtk::Core::DLogManager::registerConsoleAppender();
    qInfo() << "SparkPlayer";
    // 设置about
    a->setApplicationName("SparkPlayer");
    a->setOrganizationName("SakuraSpark");
    a->setApplicationVersion("0.1b");
    a->setApplicationDescription("A beautiful modern media player");
    DAboutDialog *about = new DAboutDialog();
    //about->setWebsiteLink("https://github.com/XiaoJiang0208");
    about->setWebsiteName("XiaoJiang's Github");
    about->setVersion(a->applicationVersion());
    about->setDescription(a->applicationDescription());
    about->setProductName(a->applicationName());
    a->setAboutDialog(about);
    
    //DApplicationSettings as;
    //Q_UNUSED(as);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SparkPlayer_" + QLocale(locale).name();
        qInfo() << baseName;
        if (translator.load(":/i18n/" + baseName)) {
            a->installTranslator(&translator);
            break;
        }
    }
    Sparkplayer w;
    w.show();
    Dtk::Widget::moveToCenter(&w);
    QHotkey hotkey_play(QKeySequence("Ctrl+Alt+P"), true, a);
	QObject::connect(&hotkey_play, &QHotkey::activated, &w, &Sparkplayer::slotPlay);
    QHotkey hotkey_previous(QKeySequence("Ctrl+Alt+,"), true, a);
	QObject::connect(&hotkey_previous, &QHotkey::activated, &w, &Sparkplayer::slotPrevious);
    QHotkey hotkey_next(QKeySequence("Ctrl+Alt+."), true, a);
	QObject::connect(&hotkey_next, &QHotkey::activated, &w, &Sparkplayer::slotNext);
    return a->exec();
}
