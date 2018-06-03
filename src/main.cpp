#include <cstdlib>

#include <QApplication>

#include "secondary_display.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    auto logger = logger::get();
    bcm_host::initialize();

    char *env = getenv("VC_DISPLAY");

    logger->info("{}", env);

    QApplication a(argc, argv);

    secondary_display display(20);
    display.setMinimumSize(800, 480);
    display.showFullScreen();

    return a.exec();
}
