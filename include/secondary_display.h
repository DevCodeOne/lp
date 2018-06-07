#pragma once

#include <memory>

#include <QTabWidget>
#include <QPushButton>

#include "dispmanx.h"
#include "qrwidget.h"

class secondary_display : public QTabWidget {
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
    dispmanx_pixmap<int32_t> m_pixmap;
    qrwidget m_qrwidget;
    QPushButton m_button;
};
