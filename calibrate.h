#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <QDialog>
#include <QObject>

class QHBoxLayout;
class QVBoxLayout;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;


class calibrate : public QDialog
{
    Q_OBJECT
    void setupDialog();
    QVBoxLayout *lMain;
    QHBoxLayout *lBoxes;
    QVBoxLayout *lMeas;
    QVBoxLayout *lRef;
    QVBoxLayout *lConf;
    QHBoxLayout *lNew;
    QHBoxLayout *lR2;
    QHBoxLayout *lBtns;

    QLabel *lblMeas;
    QLabel *lblRef;
    QLabel *lblNew;
    QLabel *lblUnit;
    QTextEdit *textMeas;
    QTextEdit *textRef;
    QPushButton *btnClear;
    QPushButton *btnRemove;
    QPushButton *btnRefresh;
    QPushButton *btnAdd;
    QLabel *lblR2;
    QLabel *lblParams;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QLineEdit *editNew;

    float k0,k1,R2;

public:
    explicit calibrate(QWidget *parent = nullptr);
    void addValue(float measurement);
    float getk0()const{return k0;}
    float getk1()const{return k1;}
    bool saved;
private slots:
    void cancelClicked();
    void saveClicked();
    void addClicked();
    void clearClicked();
    void refreshClicked();
    void editTextChanged();
signals:
    void getMeas();
};

#endif // CALIBRATE_H
