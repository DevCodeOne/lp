#include <QPainter>

#include "logger.h"
#include "secondary_display.h"

secondary_display::secondary_display(int refresh_rate, QWidget *parent) : QWidget(parent) {
    startTimer(1000 / refresh_rate);
}

void secondary_display::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.fillRect(0, 0, 1280, 720, QColor(255, 0, 20, 255));
    painter.drawText(100, 100, "Hello World");
    logger::get()->info("Update");
}

void secondary_display::timerEvent(QTimerEvent *) {
    update_display();
}

void secondary_display::update_display() {
    // TODO add check if content has changed
    QImage content = grab().toImage();

    for (int i = 0; i < content.height(); ++i) {
        QRgb *current_line = (QRgb *)content.scanLine(i);

        // TODO copy current scanline into dest
    }

    // TODO update display
}
