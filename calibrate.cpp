#include "calibrate.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>

calibrate::calibrate(QWidget *parent):
    QDialog(parent),
    saved(false)
{
    setupDialog();
}

void calibrate::setupDialog(){
    lMain = new QVBoxLayout;

    lBoxes = new QHBoxLayout;

    lMeas = new QVBoxLayout;
    lblMeas = new QLabel(tr("Masured"));
    textMeas = new QTextEdit;
    lMeas->addWidget(lblMeas);
    lMeas->addWidget(textMeas);

    lRef = new QVBoxLayout;
    lblRef = new QLabel(tr("Reference"));
    textRef = new QTextEdit;
    lRef->addWidget(lblRef);
    lRef->addWidget(textRef);

    lConf = new QVBoxLayout;
    btnClear = new QPushButton(tr("&Clear"));
    btnRemove = new QPushButton(tr("&Remove"));
    btnRefresh = new QPushButton(tr("&Refresh"));
    lConf->addWidget(btnClear);
    lConf->addWidget(btnRemove);
    lConf->addWidget(btnRefresh);

    lBoxes->addLayout(lMeas);
    lBoxes->addLayout(lRef);
    lBoxes->addLayout(lConf);

    lNew = new QHBoxLayout;
    lblNew = new QLabel(tr("New value:"));
    editNew = new QLineEdit;
    lblUnit = new QLabel("°C");
    btnAdd = new QPushButton(tr("&Add"));
    lNew->addWidget(lblNew);
    lNew->addWidget(editNew);
    lNew->addWidget(lblUnit);
    lNew->addWidget(btnAdd);

    lblR2 = new QLabel;
    lblParams = new QLabel;

    lBtns = new QHBoxLayout;
    btnCancel = new QPushButton(tr("&Cancel"));
    btnSave = new QPushButton(tr("&Save"));
    lBtns->addWidget(btnCancel);
    lBtns->addWidget(btnSave);

    lMain->addLayout(lBoxes);
    lMain->addLayout(lNew);
    lMain->addWidget(lblR2);
    lMain->addWidget(lblParams);
    lMain->addLayout(lBtns);

    this->setLayout(lMain);

    connect(btnCancel,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    connect(btnSave,SIGNAL(clicked()),this,SLOT(saveClicked()));
    connect(btnAdd,SIGNAL(clicked()),this,SLOT(addClicked()));
    connect(btnRefresh,SIGNAL(clicked()),this,SLOT(refreshClicked()));
    connect(editNew,SIGNAL(textChanged(QString)),this,SLOT(editTextChanged()));
    connect(btnClear,SIGNAL(clicked()),this,SLOT(clearClicked()));

    btnSave->setEnabled(false);
}

void calibrate::editTextChanged(){
    bool ok;
    if(!editNew->text().isEmpty()){
        qreal number = editNew->text().toDouble(&ok);
        if(abs(number) < 100 && ok == true){
        }else{
            editNew->setText("0");
        }
    }
}

void calibrate::addClicked(){
    emit getMeas();
}


void calibrate::clearClicked(){
    textMeas->clear();
    textRef->clear();
}

void calibrate::refreshClicked(){
    // spočítej parametry
    // spočítej R^2
    // jsou třeba alespoň dvě hodnoty
    auto str2float = [](auto str){
        QList<float> values;
        for (const auto& s : str){
            values.append(s.toFloat());
        }
        return values;
    };

    //sestav seznamy hodnot
    QStringList measured = textMeas->toPlainText().split("\n",Qt::SkipEmptyParts);
    QStringList reference = textRef->toPlainText().split("\n",Qt::SkipEmptyParts);

    if(measured.size() != reference.size()){
        QMessageBox::critical(this,tr("Computation error!"),tr("Sizes of given vectors does not match!"));  // what fucked up
        return;
    }
    if(measured.size() < 2){
        QMessageBox::critical(this,tr("Computation error!"),tr("Must have at least two values!"));  // what fucked up
        return;
    }

    //převeď na seznamy čísel
    QList<float> meas = str2float(measured);
    QList<float> ref = str2float(reference);
    // spočítej parametry
    float n = meas.size();
    float suma_meas = 0;
    for(const auto &m : meas)suma_meas += m;
    float suma_meas2 = 0;
    for(const auto &m : meas)suma_meas2 += m*m;
    float suma_ref = 0;
    for(const auto &r : ref)suma_ref += r;
    float suma_measref = 0;
    for(int i = 0; i < meas.size(); i++)suma_measref += meas[i]*ref[i];

    float jmenovatel = n*suma_meas2-suma_meas*suma_meas;
    k0 = suma_meas2*suma_ref-suma_meas*suma_measref;
    k0 = k0/jmenovatel;
    k1 = n*suma_measref-suma_meas*suma_ref;
    k1 = k1/jmenovatel;

    float mean = suma_ref/n;
    float SS_res = 0;
    for(int i = 0; i < ref.size(); i++)SS_res += (ref[i] - (k0 + k1*meas[i]))*(ref[i] - (k0 + k1*meas[i]));
    float SS_tot = 0;
    for(const auto &r : ref)SS_tot += (r - mean)*(r - mean);
    R2 = 1.0-(SS_res/SS_tot);
    lblR2->setText(tr("R^2 = ")+QString::number(R2,'e',5));
    lblParams->setText("k0 = "+QString::number(k0,'e',3)+", k1 = "+QString::number(k1,'e',3));

    btnSave->setEnabled(true);
}

void calibrate::addValue(float measurement){
    // přidej čísla to tabulky
    // je možné hodnoty zadávat manuálně do boxů
    textMeas->append(QString::number(measurement)+"\n");
    textRef->append(editNew->text()+"\n");
}

void calibrate::cancelClicked(){
    this->close();
}

void calibrate::saveClicked(){
    saved = true;
    this->close();
}
