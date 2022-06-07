#ifndef CONFSERIAL_H
#define CONFSERIAL_H

#include <QDialog>
#include <QObject>

class QLabel;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class confSerial : public QDialog
{
    Q_OBJECT
    int baudRate;
    QString portName;

    QVBoxLayout *lMain;
    QHBoxLayout *lBaud;
    QHBoxLayout *lPort;
    QHBoxLayout *lManufact;
    QHBoxLayout *lVendor;
    QHBoxLayout *lProduct;
    QHBoxLayout *lBtns;

    QLabel *lblBaud;
    QLabel *lblPort;
    QComboBox *comboBaud;
    QComboBox *comboPort;
    QLabel *lblTextManufact;
    QLabel *lblManufact;
    QLabel *lblTextVendor;
    QLabel *lblVendor;
    QLabel *lblTextProduct;
    QLabel *lblProduct;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    void createActions();
public:
    bool saved;
    explicit confSerial(QWidget *parent = nullptr);
    int getBaudRate()const{return baudRate;}
    void setBaudRate(int rate);
    QString getPort()const{return portName;}
    void setPort(QString port);
private slots:
    void portChanged();
    void cancelPushed();
    void savePushed();
};

#endif // CONFSERIAL_H
