#pragma once

#include <memory>

#include <QWidget>

#include "dispmanx.h"

class secondary_display : public QWidget {
    Q_OBJECT
   public:
    secondary_display(int refresh_rate, QWidget *parent = nullptr);
    void update_display();

   private:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

    std::unique_ptr<dispmanx_display> m_display = nullptr;
    std::unique_ptr<dispmanx_resource> m_resource = nullptr;
    std::unique_ptr<dispmanx_element_handle> m_element = nullptr;
};
