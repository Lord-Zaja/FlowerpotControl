#include "confserial.h"
#include <QSerialPortInfo>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
//#include <QStringList>

confSerial::confSerial(QWidget *parent):QDialog(parent),
    saved(false)
{
    this->setWindowTitle(tr("Configure serial port"));
    createActions();
}

void confSerial::createActions(){
    QSerialPortInfo info;
    QStringList ports;
    QStringList bauds;
    for(auto &port : info.availablePorts()){
        ports.append(port.portName());
    }

    lPort = new QHBoxLayout;
    lblPort = new QLabel(tr("Port name: "));
    comboPort = new QComboBox;
    comboPort->addItems(ports);
    comboPort->setCurrentIndex(0);
    lPort->addWidget(lblPort);
    lPort->addWidget(comboPort);

    info = QSerialPortInfo(comboPort->currentText());
    for(auto baud : info.standardBaudRates()){
        bauds.append(QString::number(baud));
    }
    lBaud = new QHBoxLayout;
    lblBaud = new QLabel(tr("Baud rate: "));
    comboBaud = new QComboBox;
    comboBaud->addItems(bauds);
    comboBaud->setCurrentIndex(0);
    if(ports.size() == 0)comboBaud->setEnabled(false);
    lBaud->addWidget(lblBaud);
    lBaud->addWidget(comboBaud);

    lManufact = new QHBoxLayout;
    lblTextManufact = new QLabel(tr("Manufacturer: "));
    lblManufact = new QLabel(info.manufacturer());
    lManufact->addWidget(lblTextManufact);
    lManufact->addWidget(lblManufact);

    lVendor = new QHBoxLayout;
    lblTextVendor = new QLabel(tr("Vendor ID: "));
    lblVendor = new QLabel(QString::number(info.vendorIdentifier()));
    lVendor->addWidget(lblTextVendor);
    lVendor->addWidget(lblVendor);

    lProduct = new QHBoxLayout;
    lblTextProduct = new QLabel(tr("Product ID: "));
    lblProduct = new QLabel(QString::number(info.productIdentifier()));
    lProduct->addWidget(lblTextProduct);
    lProduct->addWidget(lblProduct);

    lBtns = new QHBoxLayout;
    btnCancel = new QPushButton(tr("&Cancel"));
    btnSave = new QPushButton(tr("&Save"));
    lBtns->addWidget(btnCancel);
    lBtns->addWidget(btnSave);
    if(comboPort->currentText()=="")btnSave->setEnabled(false);

    lMain = new QVBoxLayout;
    lMain->addLayout(lPort);
    lMain->addLayout(lBaud);
    lMain->addLayout(lManufact);
    lMain->addLayout(lVendor);
    lMain->addLayout(lProduct);
    lMain->addLayout(lBtns);
    this->setLayout(lMain);

    connect(comboPort,SIGNAL(currentIndexChanged(int)),this,SLOT(portChanged()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(cancelPushed()));
    connect(btnSave,SIGNAL(clicked()),this,SLOT(savePushed()));
}

void confSerial::setBaudRate(int rate){
    comboBaud->setCurrentText(QString::number(rate));
}

void confSerial::setPort(QString port){
    comboPort->setCurrentText(port);
}

void confSerial::portChanged(){
    QStringList bauds;
    if(comboPort->currentText()==""){
        btnSave->setEnabled(false);
        comboBaud->setEnabled(false);
    }else{
        btnSave->setEnabled(true);
        comboBaud->setEnabled(true);
        QSerialPortInfo info = QSerialPortInfo(comboPort->currentText());
        for(auto baud : info.standardBaudRates()){
            bauds.append(QString::number(baud));
        }
        comboBaud->clear();
        comboBaud->addItems(bauds);
        comboBaud->setCurrentIndex(0);
    }
}

void confSerial::cancelPushed(){
    this->close();
}

void confSerial::savePushed(){
    saved = true;
    portName = comboPort->currentText();
    baudRate = comboBaud->currentText().toInt();
    this->close();
}
