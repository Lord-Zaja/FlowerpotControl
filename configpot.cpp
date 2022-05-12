#include "configpot.h"
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

configPot::configPot(QWidget *parent):QDialog(parent),
    saved(false),
    modeWatter(2),
    valueHum(1),
    modesWatter({"Keep cup full","Keep cup half full","Keep soil humidity at:"})
{
    setupActions();
}

void configPot::setupActions(){
    lKeepTemp = new QHBoxLayout;
    lblKeepTemp = new QLabel(tr("Keep constant temperature:"));
    checkKeepTemp = new QCheckBox;
    checkKeepTemp->setChecked(keepTemp);
    lKeepTemp->addWidget(lblKeepTemp);
    lKeepTemp->addWidget(checkKeepTemp);

    lKeepTempValue = new QHBoxLayout;
    lblKeepTemp = new QLabel(tr("Constant temperature: "));
    spinTemp = new QSpinBox;
    spinTemp->setRange(0,30);
    spinTemp->setValue(temp);
    if(!checkKeepTemp->isChecked())spinTemp->setEnabled(false);
    lKeepTempValue->addWidget(lblKeepTemp);
    lKeepTempValue->addWidget(spinTemp);

    lMonday = new QHBoxLayout;
    comboMode = new QComboBox;
    comboMode->addItems(modesWatter);
    comboMode->setCurrentIndex(modeWatter);
    spinValue = new QSpinBox;
    spinValue->setRange(0,100);
    spinValue->setValue(valueHum);
    if(modeWatter != 2)spinValue->setEnabled(false);
    lblUnit = new QLabel("%");
    lMonday->addWidget(comboMode);
    lMonday->addWidget(spinValue);
    lMonday->addWidget(lblUnit);

    lBtns = new QHBoxLayout;
    btnCancel = new QPushButton(tr("&Calcel"));
    btnSave = new QPushButton(tr("&Save"));
    lBtns->addWidget(btnCancel);
    lBtns->addWidget(btnSave);

    lMain = new QVBoxLayout;
    lMain->addLayout(lKeepTemp);
    lMain->addLayout(lKeepTempValue);
    lMain->addLayout(lMonday);
    lMain->addLayout(lBtns);

    connect(checkKeepTemp,SIGNAL(stateChanged(int)),this,SLOT(keepTempChanged(int)));
    connect(spinTemp,SIGNAL(valueChanged(int)),this,SLOT(valueTempChanged(int)));
    connect(comboMode,SIGNAL(currentIndexChanged(int)),this,SLOT(modeWatterChanged(int)));
    connect(spinValue,SIGNAL(valueChanged(int)),this,SLOT(valueHumChanged(int)));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    connect(btnSave,SIGNAL(clicked()),this,SLOT(saveClicked()));

    this->setLayout(lMain);
}

void configPot::keepTempChanged(int state){
    if(state == Qt::Checked){
        spinTemp->setEnabled(true);
        keepTemp = 1;
    }else{
       spinTemp->setEnabled(false);
       keepTemp = 0;
    }
}

void configPot::valueTempChanged(int value){
    temp = value;
}

void configPot::modeWatterChanged(int idx){
    modeWatter = idx;
    if(idx == 2){
        spinValue->setEnabled(true);
    }else{
        spinValue->setEnabled(false);
    }
}

void configPot::valueHumChanged(int value){
    valueHum = value;
}

void configPot::cancelClicked(){
    this->close();
}

void configPot::saveClicked(){
    saved = true;
    this->close();
}

void configPot::setKeepTemp(bool state){
    this->keepTemp = state;
    checkKeepTemp->setChecked(keepTemp);
}

void configPot::setTemp(int temperature){
    this->temp = temperature;
    spinTemp->setValue(temp);
}

void configPot::setMode(int mode){
    this->modeWatter = mode;
    comboMode->setCurrentIndex(modeWatter);
}

void configPot::setHum(int humidity){
    this->valueHum = humidity;
    spinValue->setValue(valueHum);
}



configPot::~configPot()
{
}


