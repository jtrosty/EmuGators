#include "window.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("NES Emulator");
    QApplication::setApplicationVersion("!Pre-alpha, if that!");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("NES Emulator");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("ROM", QApplication::translate("main", "ROM to load into the emulator"));

    parser.process(a);
    
    const QStringList args = parser.positionalArguments();

    if (args.length() == 0)
	goto noROM; // only a temporary hack until we finish testing the Qt frontend

noROM:
    Window w;
    w.show();
    return a.exec();
}
