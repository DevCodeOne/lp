#include <memory>
#include <string>

#include <qrencode.h>
#include <QWidget>

/** \brief This is a gui object which displays a qrcode.
 */
class qrwidget : public QWidget {
    Q_OBJECT
   public:
    /** \brief Create a qrwidget with the provided message which is then displayed by the qrcode.
     */
    qrwidget(const std::string &message = "");

    /** \brief Returns the message which is displayed by the qrcode.
     */
    const std::string &message() const;
    /**  \brief Set the message which is displayed by the qrcode.
     */
    qrwidget &message(const std::string &message);

    /** \brief Size in pixels of one square.
     */
    static inline constexpr size_t square_size = 10;

   private:
    /** \brief Method which gets called everytime the component should be redrawn.
     */
    void paintEvent(QPaintEvent *event);
    /** \brief Method to update the qrcode.
     * The reprenstation of the qrcode is lazy, which means the array of pixels is not updated
     * everytime the message is changed. Instead it gets updated once it is needed, e.g. redrawing
     * of the component
     */
    void update_qrcode();

    /** \brief The message which is encoded in the qrcode.
     */
    std::string m_message = "";
    /** \brief Pixel data of the qrcode.
     */
    std::unique_ptr<unsigned char[]> m_data = nullptr;
    /** \brief Number of pixels in x and y direction.
     * The size of the qrcode is dependant on the length of the message so this number represents
     * the side_length of the qrcode in either direction, since qrcodes are squares.
     */
    int m_side_length = 0;
};
