#ifndef CONFIGPOT_H
#define CONFIGPOT_H

#include <QDialog>

class QComboBox;
class QLabel;
class QSpinBox;
class QHBoxLayout;
class QCheckBox;
class QVBoxLayout;
class QLineEdit;
class QPushButton;

class configPot : public QDialog
{
    Q_OBJECT
public:
    explicit configPot(QWidget *parent = nullptr);
    virtual ~configPot();
    void setKeepTemp(bool state);
    int getKeepTemp()const{return keepTemp;}
    void setTemp(int temperature);
    int getTemp()const{return temp;}
    void setMode(int mode);
    int getMode()const{return modeWatter;}
    void setHum(int humidity);
    int getHum()const{return this->valueHum;}
    bool saved;
private:
    void setupActions();
    QVBoxLayout *lMain;

    QHBoxLayout *lKeepTemp;
    QLabel *lblKeepTemp;
    QCheckBox *checkKeepTemp;
    QHBoxLayout *lKeepTempValue;
    QLabel *lblKeepValue;
    QSpinBox *spinTemp;

    QHBoxLayout *lMonday;
    QComboBox *comboMode;
    QSpinBox *spinValue;
    QLabel *lblUnit;

    QHBoxLayout *lBtns;
    QPushButton *btnCancel;
    QPushButton *btnSave;

    int keepTemp;
    int temp;
    int modeWatter;
    int valueHum;
    QStringList modesWatter;
private slots:
    void keepTempChanged(int state);
    void valueTempChanged(int value);
    void modeWatterChanged(int idx);
    void valueHumChanged(int value);
    void cancelClicked();
    void saveClicked();
};

#endif // CONFIGPOT_H
