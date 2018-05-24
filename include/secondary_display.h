#pragma once

#include <QWidget>

class secondary_display : public QWidget {
    Q_OBJECT
   public:
    secondary_display(int refresh_rate, QWidget *parent = nullptr);

   private:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void update_display();
};
