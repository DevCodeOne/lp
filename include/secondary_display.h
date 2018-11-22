#pragma once

#include <memory>

#include <QPushButton>
#include <QTabWidget>

#include "dispmanx/dispmanx_wrapper.h"
#include "qrwidget.h"

/** \brief This class controls the secondary display, which is by default the Raspberry Pi LCD
 * connected with the ribbon cable.
 */
class secondary_display : public QTabWidget {
    Q_OBJECT
   public:
    /** \brief Construct a display with a specific refresh rate at which the display gets checked
     * for changes.
     */
    secondary_display(int refresh_rate);
    /** \brief Update the contents of the display.
     * Check if the display has to be updated. If it has to be updated repaint the whole gui take a
     * screenshot of the display and copy the pixels from the QImage to the display.
     */
    void update_display();

   private:
    /** \brief Make a screenshot of the gui and put it in a QImage with the right pixel format.
     */
    QImage pixels();
    /** \brief Gets called everytime the widget is repainted.
     * The widget will only be repainted when it is needed.
     */
    void paintEvent(QPaintEvent *event);
    /** \brief Gets called periodicly.
     * The period is configured with the refresh rate provided in the constructor.
     */
    void timerEvent(QTimerEvent *event);

    std::unique_ptr<bcm_host_wrapper::dispmanx_display> m_display = nullptr;
    /** \brief Resource which will be displayed on the screen.
     * This resource will contain the pixels from the screenshot of the gui.
     */
    std::unique_ptr<bcm_host_wrapper::dispmanx_resource> m_resource = nullptr;
    /** \brief This is an handle to an element which is added to an update.
     * It will display the resource attacted to it.
     */
    std::unique_ptr<bcm_host_wrapper::dispmanx_element_handle> m_element = nullptr;
    /** \brief This variable will be true when the contents of the screen and the gui differ.
     * It will indicate if the dispmanx_pixmap has to be copied to the screen.
     */
    bool m_update_needed = true;
    /** \brief This variable indicates if the widget is repainted because of the pixels() method.
     */
    bool m_is_grabbing = false;
    /** \brief The buffer in which the screenshot will be stored in.
     */
    bcm_host_wrapper::dispmanx_pixmap<int32_t> m_pixmap;
    qrwidget m_qrwidget;
    QPushButton m_button;
};
