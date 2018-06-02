#include <string>
#include <memory>

#include <qrencode.h>
#include <QWidget>

class qrwidget : public QWidget {
    Q_OBJECT
   public:
    qrwidget(const std::string &message = "");

    const std::string &message() const;
    qrwidget &message(const std::string &message);

   private:
    void paintEvent(QPaintEvent *event);
    void update_qrcode();

    std::string m_message = "";
    std::unique_ptr<unsigned char []> m_data = nullptr;
    int m_side_length = 0;

};
