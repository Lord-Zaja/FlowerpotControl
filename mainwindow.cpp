#include "mainwindow.h"
#include <cstdio>
#include <random>

#include <QDebug>
#include <QtCharts/QChartView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDateTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <QMenu>
#include <QToolBar>
#include <QDial>
#include <QQuickView>
#include <QQuickItem>
#include <QTabWidget>
#include <QTabBar>
#include <QProgressBar>
#include <QAbstractAxis>
#include <QFrame>
#include <QLineEdit>
#include <QStatusBar>
#include <QMenuBar>

#include "chart.h"
#include "configpot.h"
#include "mqtt_client.h"
#include <mosquittopp.h>

#define PUBLISH_TOPIC "v3/flowerpot-arduino@ttn/devices/eui-e0080e1010101010/down/push"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      keepTemp(false),
      setTemp(20),
      setHum(50),
      mode(2)
{
    setupForm();
    setupTabs();
    setupActions();
    setupMQTT();


    //Testovací náhodná data pro loru
    distribution = std::normal_distribution<double>(5.0,5.0);
    timerTest = new QTimer;
    timerTest->setInterval(5000);
    connect(timerTest, SIGNAL(timeout()), this, SLOT(test()));

}

void MainWindow::setupForm(){
    this->setWindowTitle(tr("Flowerpot Control"));
    QIcon iconWindow = QIcon(":/icons/pot.png");
    this->setWindowIcon(iconWindow);
    this->resize(800,600);
    // Toolbar
    bar = new QToolBar;
    static const int SIZE = 20;
    static const QString greenSS = QString("color: white;border-radius: %1;background-color: qlineargradient(spread:pad, x1:0.145, y1:0.16, x2:1, y2:1, stop:0 rgba(20, 252, 7, 255), stop:1 rgba(25, 134, 5, 255));").arg(SIZE/2);
    static const QString redSS = QString("color: white;border-radius: %1;background-color: qlineargradient(spread:pad, x1:0.145, y1:0.16, x2:0.92, y2:0.988636, stop:0 rgba(255, 12, 12, 255), stop:0.869347 rgba(103, 0, 0, 255));").arg(SIZE/2);
    static const QString orangeSS = QString("color: white;border-radius: %1;background-color: qlineargradient(spread:pad, x1:0.232, y1:0.272, x2:0.98, y2:0.959773, stop:0 rgba(255, 113, 4, 255), stop:1 rgba(91, 41, 7, 255))").arg(SIZE/2);
    static const QString blueSS = QString("color: white;border-radius: %1;background-color: qlineargradient(spread:pad, x1:0.04, y1:0.0565909, x2:0.799, y2:0.795, stop:0 rgba(203, 220, 255, 255), stop:0.41206 rgba(0, 115, 255, 255), stop:1 rgba(0, 49, 109, 255));").arg(SIZE/2);
    //this->addToolBar(bar);

    led = new QLabel("   ");
    led->setStyleSheet(redSS);

    bar->addWidget(led);
}

void MainWindow::setupActions(){
    // Menu
    mnuConf = new QMenu(tr("&Configure"));
    actLoraConf = new QAction(tr("&COnfigure LoRa"));
    //mnuConf->addAction(actLoraConf);
    actLoraConnect = new QAction(tr("&Connect to LoRa"));

    actPotConf = new QAction(tr("&Pot config"));
    this->menuBar()->addMenu(mnuConf);
    QIcon iconConfig = QIcon::fromTheme("document-properties",QIcon(":/icons/wrench.png"));
    actPotConf->setIcon(iconConfig);
    mnuConf->addAction(actPotConf);

    //Tabs
    wTabs = new QTabWidget;
    wTabs->setTabPosition(QTabWidget::North);
    wTabs->addTab(wStats,tr("&Stats"));
    wTabs->addTab(wTemp,tr("&Temperature"));
    wTabs->addTab(wHum,tr("&Humidity"));
    wTabs->addTab(wLight,tr("&Light"));
    wTabs->addTab(wSoil,tr("&Soil humidity"));
    wTabs->addTab(wCup,tr("&Cup watter"));
    wTabs->addTab(wRez,tr("&Rez. watter"));
    wTabs->addTab(wTest,tr("&Test rand data"));

    //areaMain = new QScrollArea;
    //areaMain->setWidget(widgetMain);
    this->setCentralWidget(wTabs);


    //printTemp(25);
    //printLight(50);
    //printLight(75);
/*
    chartLight->addPoint(0.5,0.5);
    chartLight->addPoint(0.75,0.5);
    chartLight->addPoint(1,1);
    chartLight->addPoint(1.25,1.5);
    chartLight->addPoint(1.5,0.7);*/

    /*
    testChart = new Chart;
    testChart->setTitle(tr("Test chart"));
    testChart->legend()->hide();
    testChart->setAnimationOptions(QChart::AllAnimations);
    testChartView = new QChartView(testChart);
    testChartView->setRenderHint(QPainter::Antialiasing);
    this->setCentralWidget(testChartView);
    this->resize(400, 300);

    // body mi mělí jít v x ose za sebou .. spline se napojuje bod za bodem .. podle toho jak jsou přidány
    // možná by to šlo opravit funkcí resetSeries .. kde by se series vynulovala .. počkalo by se
    // kvůli tomu, aby se graf sám aktualizoval a pak by se tam vše znovu zapsalo
    testChart->addPoint(0.5,0.5);
    testChart->addPoint(0.75,0.5);
    testChart->addPoint(1,1);
    testChart->addPoint(1.25,1.5);
    //testChart->addPoint(0.7,0.7);
*/

    connect(actPotConf,SIGNAL(triggered()),this,SLOT(confPot()));
    printSetValues();

    this->show();
    this->statusBar()->showMessage(tr("Ready.."),2000);
}

void MainWindow::printSetValues(){
    QPalette palTemp;
    QPalette palHum;
    // Temperature
    QString temp = QString::number(setTemp) + "°C";
    lblTempDaySetValue->setText(temp);
    if(keepTemp == 1){
        palTemp.setColor(QPalette::Shadow, Qt::yellow);
        fTempDay->setPalette(palTemp);
    }else{
        palTemp.setColor(QPalette::Window,Qt::gray);
        fTempDay->setPalette(palTemp);
    }
    // Humidity
    QString hum = QString::number(setHum) + "%";
    lblSoilDaySetValue->setText(hum);
    if(mode == 2){
        palHum.setColor(QPalette::Shadow, Qt::yellow);
        fSoilDay->setPalette(palHum);
    }else{
        palHum.setColor(QPalette::Window, Qt::gray);
        fSoilDay->setPalette(palHum);
    }

}

void MainWindow::setupTabs(){
    QPalette pal;
    QString styleFont;
    // Stats tab -----------------------------------------------------------------------------------------
    lStatsDials = new QHBoxLayout;
    // Temperature dial
    viewDialTemp = new QQuickView();
    viewDialTemp->setSource(QUrl("qrc:/qml/Dial.qml"));
    viewDialTemp->rootObject()->setProperty("value",0);
    contDialTemp = QWidget::createWindowContainer(viewDialTemp, this);
    contDialTemp->setMinimumSize(200, 200);
    contDialTemp->setMaximumSize(200, 200);
    contDialTemp->setFocusPolicy(Qt::TabFocus);
    lblStatsTemp = new QLabel(tr("Temperature"));
    lblStatsTemp->setStyleSheet("color: black; font-size: 20px");
    lblStatsTempValue = new QLabel(tr("0°C"));
    lblStatsTempValue->setStyleSheet("color: black; font-size: 40px");
    lStatsTemp = new QVBoxLayout;
    //lStatsTemp->setAlignment();
    lStatsTemp->addWidget(lblStatsTempValue,0,Qt::AlignHCenter);
    lStatsTemp->addWidget(contDialTemp);
    lStatsTemp->addWidget(lblStatsTemp);
    lStatsDials->addLayout(lStatsTemp);
    // Humidity dial
    viewDialHum = new QQuickView();
    viewDialHum->setSource(QUrl("qrc:/qml/Dial.qml"));
    viewDialHum->rootObject()->setProperty("value",0);
    contDialHum = QWidget::createWindowContainer(viewDialHum, this);
    contDialHum->setMinimumSize(200, 200);
    contDialHum->setMaximumSize(200, 200);
    contDialHum->setFocusPolicy(Qt::TabFocus);
    lblStatsHum = new QLabel(tr("Humidity"));
    lblStatsHum->setStyleSheet("color: black; font-size: 20px");
    lblStatsHumValue = new QLabel(tr("0%"));
    lblStatsHumValue->setStyleSheet("color: black; font-size: 40px");
    lStatsHum = new QVBoxLayout;
    lStatsHum->addWidget(lblStatsHumValue,0,Qt::AlignHCenter);
    lStatsHum->addWidget(contDialHum);
    lStatsHum->addWidget(lblStatsHum);
    lStatsDials->addLayout(lStatsHum);
    // Light dial
    viewDialLight = new QQuickView();
    viewDialLight->setSource(QUrl("qrc:/qml/Dial.qml"));
    viewDialLight->rootObject()->setProperty("value",0);
    contDialLight = QWidget::createWindowContainer(viewDialLight, this);
    contDialLight->setMinimumSize(200, 200);
    contDialLight->setMaximumSize(200, 200);
    contDialLight->setFocusPolicy(Qt::TabFocus);
    lblStatsLight = new QLabel(tr("Light"));
    lblStatsLight->setStyleSheet("color: black; font-size: 20px");
    lblStatsLightValue = new QLabel(tr("0%"));
    lblStatsLightValue->setStyleSheet("color: black; font-size: 40px");
    lStatsLight = new QVBoxLayout;
    lStatsLight->addWidget(lblStatsLightValue,0,Qt::AlignHCenter);
    lStatsLight->addWidget(contDialLight);
    lStatsLight->addWidget(lblStatsLight);
    lStatsDials->addLayout(lStatsLight);
    // Soil dial
    viewDialSoil = new QQuickView();
    viewDialSoil->setSource(QUrl("qrc:/qml/Dial.qml"));
    viewDialSoil->rootObject()->setProperty("value",0);
    contDialSoil = QWidget::createWindowContainer(viewDialSoil, this);
    contDialSoil->setMinimumSize(200, 200);
    contDialSoil->setMaximumSize(200, 200);
    contDialSoil->setFocusPolicy(Qt::TabFocus);
    lblStatsSoil = new QLabel(tr("Soil humidity"));
    lblStatsSoil->setStyleSheet("color: black; font-size: 20px");
    lblStatsSoilValue = new QLabel(tr("0%"));
    lblStatsSoilValue->setStyleSheet("color: black; font-size: 40px");
    lStatsSoil = new QVBoxLayout;
    lStatsSoil->addWidget(lblStatsSoilValue,0,Qt::AlignHCenter);
    lStatsSoil->addWidget(contDialSoil);
    lStatsSoil->addWidget(lblStatsSoil);
    lStatsDials->addLayout(lStatsSoil);
    // Watter cup progressbar
    progStatsWatterCup = new QProgressBar;
    progStatsWatterCup->setOrientation(Qt::Vertical);
    progStatsWatterCup->setRange(0,100);
    pal = QPalette();
    pal.setColor(QPalette::Highlight,Qt::blue);
    progStatsWatterCup->setPalette(pal);
    lblStatsWatterCup = new QLabel(tr("Cup level"));
    lblStatsWatterCup->setStyleSheet("color: black; font-size: 20px");
    lblStatsWatterCupValue = new QLabel(tr("0%"));
    lblStatsWatterCupValue->setStyleSheet("color: black; font-size: 40px");
    lStatsWatterCup = new QVBoxLayout;
    lStatsWatterCup->addWidget(lblStatsWatterCupValue);
    lStatsWatterCup->addWidget(progStatsWatterCup);
    lStatsWatterCup->addWidget(lblStatsWatterCup);
    lStatsDials->addLayout(lStatsWatterCup);
    // Watter rezervoar progressbar
    progStatsWatterRez = new QProgressBar;
    progStatsWatterRez->setOrientation(Qt::Vertical);
    progStatsWatterRez->setRange(0,100);
    progStatsWatterRez->setPalette(pal);
    lblStatsWatterRez = new QLabel(tr("Rez. level "));
    lblStatsWatterRez->setStyleSheet("color: black; font-size: 20px");
    lblStatsWatterRezValue = new QLabel(tr("0%"));
    lblStatsWatterRezValue->setStyleSheet("color: black; font-size: 40px");
    lStatsWatterRez = new QVBoxLayout;
    lStatsWatterRez->addWidget(lblStatsWatterRezValue);
    lStatsWatterRez->addWidget(progStatsWatterRez);
    lStatsWatterRez->addWidget(lblStatsWatterRez);
    lStatsDials->addLayout(lStatsWatterRez);
    // Setup Stats widget, layout
    wStats = new QWidget;
    wStats->setLayout(lStatsDials);
    pal = QPalette();
    pal.setColor(QPalette::Window, Qt::white);
    wStats->setAutoFillBackground(true);
    wStats->setPalette(pal);

    // Temperature tab -----------------------------------------------------------------------------------------
    chartTemp = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartTemp->setTitle((tr("Temperature")));
    chartTemp->legend()->hide();
    chartTemp->setAnimationOptions(QChart::AllAnimations);
    chartTemp->setAxisYLabel(tr("T [°C]"));
    viewTemp = new QChartView(chartTemp);
    viewTemp->setRenderHint(QPainter::Antialiasing);
    //viewTemp->setSceneRect(QRectF(0,0,700,400));
    viewTemp->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblTemp = new QLabel(tr("Temperature: "));
    lblTemp->setStyleSheet(styleFont);
    lblTempValue = new QLabel(tr("0°C"));
    lblTempValue->setStyleSheet(styleFont);
    lblTempDiv = new QLabel(tr("t_div: "));
    lblTempDiv->setStyleSheet(styleFont);
    editTempDiv = new QLineEdit;
    editTempDiv->setFixedWidth(100);
    editTempDiv->setFixedHeight(30);
    editTempDiv->setText("1");
    editTempDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblTempDivValue = new QLabel(tr("hour"));
    lblTempDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblTempDaySet = new QLabel(tr("Temperature set at: "));
    lblTempDaySet->setStyleSheet(styleFont);
    lblTempDaySetValue = new QLabel(tr("0°C"));
    lblTempDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblTempNightSet = new QLabel(tr("Temperature set at: "));
    lblTempNightSet->setStyleSheet(styleFont);
    lblTempNightSetValue = new QLabel(tr("0°C"));
    lblTempNightSetValue->setStyleSheet(styleFont);
    // Setup Temp widget, layout
    lTemp = new QHBoxLayout;
    lTempLabels = new QVBoxLayout;
    lTempLblValue = new QHBoxLayout;            // Actual value
    lTempLblValue->addWidget(lblTemp);
    lTempLblValue->addWidget(lblTempValue);
    lTempLabels->addLayout(lTempLblValue);
    lTempLblDiv = new QHBoxLayout;              // Shown history
    lTempLblDiv->addWidget(lblTempDiv);
    lTempLblDiv->addWidget(editTempDiv);
    lTempLblDiv->addWidget(lblTempDivValue);
    lTempLabels->addLayout(lTempLblDiv);
    // Day frame
    fTempDay = new QFrame;
    lTempDayLabels = new QVBoxLayout;
    lTempDayLblSet = new QHBoxLayout;
    lTempDayLabels->addWidget(lblTempDay);
    lTempDayLblSet->addWidget(lblTempDaySet);         // Wanted value
    lTempDayLblSet->addWidget(lblTempDaySetValue);
    lTempDayLabels->addLayout(lTempDayLblSet);      // Put labels into labels layout
    fTempDay->setLayout(lTempDayLabels);
    lTempLabels->addWidget(fTempDay);
    // Night frame
    fTempNight = new QFrame;
    lTempNightLabels = new QVBoxLayout;
    lTempNightLblSet = new QHBoxLayout;
    lTempNightLabels->addWidget(lblTempNight);
    lTempNightLblSet->addWidget(lblTempNightSet);         // Wanted value
    lTempNightLblSet->addWidget(lblTempNightSetValue);
    lTempNightLabels->addLayout(lTempNightLblSet);      // Put labels into labels layout
    fTempNight->setLayout(lTempNightLabels);
    lTempLabels->addWidget(fTempNight);
    // Setup layouts
    fTempDay->setFrameShape(QFrame::WinPanel);
    fTempDay->setBackgroundRole(QPalette::Shadow);
    fTempDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fTempDay->setAutoFillBackground(true);
    fTempDay->setPalette(pal);
    fTempNight->setFrameShape(QFrame::WinPanel);
    fTempNight->setBackgroundRole(QPalette::Shadow);
    fTempNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fTempNight->setAutoFillBackground(true);
    fTempNight->setPalette(pal);
    lTemp->addWidget(viewTemp);            // Put chart with labels into main H layout
    lTemp->addLayout(lTempLabels);
    wTemp = new QWidget;
    wTemp->setLayout(lTemp);
    pal.setColor(QPalette::Window, Qt::white);
    wTemp->setAutoFillBackground(true);
    wTemp->setPalette(pal);
    // Setup signals
    connect(editTempDiv,SIGNAL(textChanged(QString)),this,SLOT(tempDivChanged()));

    // Humidity tab -----------------------------------------------------------------------------------------
    chartHum = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartHum->setTitle((tr("Humerature")));
    chartHum->legend()->hide();
    chartHum->setAnimationOptions(QChart::AllAnimations);
    chartHum->setAxisYLabel(tr("H [%]"));
    viewHum = new QChartView(chartHum);
    viewHum->setRenderHint(QPainter::Antialiasing);
    //viewHum->setSceneRect(QRectF(0,0,700,400));
    viewHum->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblHum = new QLabel(tr("Humidity: "));
    lblHum->setStyleSheet(styleFont);
    lblHumValue = new QLabel(tr("0°C"));
    lblHumValue->setStyleSheet(styleFont);
    lblHumDiv = new QLabel(tr("t_div: "));
    lblHumDiv->setStyleSheet(styleFont);
    editHumDiv = new QLineEdit;
    editHumDiv->setFixedWidth(100);
    editHumDiv->setFixedHeight(30);
    editHumDiv->setText("1");
    editHumDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblHumDivValue = new QLabel(tr("hour"));
    lblHumDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblHumDaySet = new QLabel(tr("Humidity set at: "));
    lblHumDaySet->setStyleSheet(styleFont);
    lblHumDaySetValue = new QLabel(tr("0°C"));
    lblHumDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblHumNightSet = new QLabel(tr("Humidity set at: "));
    lblHumNightSet->setStyleSheet(styleFont);
    lblHumNightSetValue = new QLabel(tr("0°C"));
    lblHumNightSetValue->setStyleSheet(styleFont);
    // Setup Hum widget, layout
    lHum = new QHBoxLayout;
    lHumLabels = new QVBoxLayout;
    lHumLblValue = new QHBoxLayout;            // Actual value
    lHumLblValue->addWidget(lblHum);
    lHumLblValue->addWidget(lblHumValue);
    lHumLabels->addLayout(lHumLblValue);
    lHumLblDiv = new QHBoxLayout;              // Shown history
    lHumLblDiv->addWidget(lblHumDiv);
    lHumLblDiv->addWidget(editHumDiv);
    lHumLblDiv->addWidget(lblHumDivValue);
    lHumLabels->addLayout(lHumLblDiv);
    // Day frame
    fHumDay = new QFrame;
    lHumDayLabels = new QVBoxLayout;
    lHumDayLblSet = new QHBoxLayout;
    lHumDayLabels->addWidget(lblTempDay);
    lHumDayLblSet->addWidget(lblHumDaySet);         // Wanted value
    lHumDayLblSet->addWidget(lblHumDaySetValue);
    lHumDayLabels->addLayout(lHumDayLblSet);      // Put labels into labels layout
    fHumDay->setLayout(lHumDayLabels);
    lHumLabels->addWidget(fHumDay);
    // Night frame
    fHumNight = new QFrame;
    lHumNightLabels = new QVBoxLayout;
    lHumNightLblSet = new QHBoxLayout;
    lHumNightLabels->addWidget(lblTempNight);
    lHumNightLblSet->addWidget(lblHumNightSet);         // Wanted value
    lHumNightLblSet->addWidget(lblHumNightSetValue);
    lHumNightLabels->addLayout(lHumNightLblSet);      // Put labels into labels layout
    fHumNight->setLayout(lHumNightLabels);
    lHumLabels->addWidget(fHumNight);
    // Setup layouts
    fHumDay->setFrameShape(QFrame::WinPanel);
    fHumDay->setBackgroundRole(QPalette::Shadow);
    fHumDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fHumDay->setAutoFillBackground(true);
    fHumDay->setPalette(pal);
    fHumNight->setFrameShape(QFrame::WinPanel);
    fHumNight->setBackgroundRole(QPalette::Shadow);
    fHumNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fHumNight->setAutoFillBackground(true);
    fHumNight->setPalette(pal);
    lHum->addWidget(viewHum);            // Put chart with labels into main H layout
    lHum->addLayout(lHumLabels);
    wHum = new QWidget;
    wHum->setLayout(lHum);
    pal.setColor(QPalette::Window, Qt::white);
    wHum->setAutoFillBackground(true);
    wHum->setPalette(pal);
    // Setup signals
    connect(editHumDiv,SIGNAL(textChanged(QString)),this,SLOT(humDivChanged()));

    // Light tab -----------------------------------------------------------------------------------------
    chartLight = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartLight->setTitle((tr("Lighterature")));
    chartLight->legend()->hide();
    chartLight->setAnimationOptions(QChart::AllAnimations);
    chartLight->setAxisYLabel(tr("L [%]"));
    viewLight = new QChartView(chartLight);
    viewLight->setRenderHint(QPainter::Antialiasing);
    //viewLight->setSceneRect(QRectF(0,0,700,400));
    viewLight->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblLight = new QLabel(tr("Light: "));
    lblLight->setStyleSheet(styleFont);
    lblLightValue = new QLabel(tr("0°C"));
    lblLightValue->setStyleSheet(styleFont);
    lblLightDiv = new QLabel(tr("t_div: "));
    lblLightDiv->setStyleSheet(styleFont);
    editLightDiv = new QLineEdit;
    editLightDiv->setFixedWidth(100);
    editLightDiv->setFixedHeight(30);
    editLightDiv->setText("1");
    editLightDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblLightDivValue = new QLabel(tr("hour"));
    lblLightDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblLightDaySet = new QLabel(tr("Light set at: "));
    lblLightDaySet->setStyleSheet(styleFont);
    lblLightDaySetValue = new QLabel(tr("0°C"));
    lblLightDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblLightNightSet = new QLabel(tr("Light set at: "));
    lblLightNightSet->setStyleSheet(styleFont);
    lblLightNightSetValue = new QLabel(tr("0°C"));
    lblLightNightSetValue->setStyleSheet(styleFont);
    // Setup Light widget, layout
    lLight = new QHBoxLayout;
    lLightLabels = new QVBoxLayout;
    lLightLblValue = new QHBoxLayout;            // Actual value
    lLightLblValue->addWidget(lblLight);
    lLightLblValue->addWidget(lblLightValue);
    lLightLabels->addLayout(lLightLblValue);
    lLightLblDiv = new QHBoxLayout;              // Shown history
    lLightLblDiv->addWidget(lblLightDiv);
    lLightLblDiv->addWidget(editLightDiv);
    lLightLblDiv->addWidget(lblLightDivValue);
    lLightLabels->addLayout(lLightLblDiv);
    // Day frame
    fLightDay = new QFrame;
    lLightDayLabels = new QVBoxLayout;
    lLightDayLblSet = new QHBoxLayout;
    lLightDayLabels->addWidget(lblTempDay);
    lLightDayLblSet->addWidget(lblLightDaySet);         // Wanted value
    lLightDayLblSet->addWidget(lblLightDaySetValue);
    lLightDayLabels->addLayout(lLightDayLblSet);      // Put labels into labels layout
    fLightDay->setLayout(lLightDayLabels);
    lLightLabels->addWidget(fLightDay);
    // Night frame
    fLightNight = new QFrame;
    lLightNightLabels = new QVBoxLayout;
    lLightNightLblSet = new QHBoxLayout;
    lLightNightLabels->addWidget(lblTempNight);
    lLightNightLblSet->addWidget(lblLightNightSet);         // Wanted value
    lLightNightLblSet->addWidget(lblLightNightSetValue);
    lLightNightLabels->addLayout(lLightNightLblSet);      // Put labels into labels layout
    fLightNight->setLayout(lLightNightLabels);
    lLightLabels->addWidget(fLightNight);
    // Setup layouts
    fLightDay->setFrameShape(QFrame::WinPanel);
    fLightDay->setBackgroundRole(QPalette::Shadow);
    fLightDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fLightDay->setAutoFillBackground(true);
    fLightDay->setPalette(pal);
    fLightNight->setFrameShape(QFrame::WinPanel);
    fLightNight->setBackgroundRole(QPalette::Shadow);
    fLightNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fLightNight->setAutoFillBackground(true);
    fLightNight->setPalette(pal);
    lLight->addWidget(viewLight);            // Put chart with labels into main H layout
    lLight->addLayout(lLightLabels);
    wLight = new QWidget;
    wLight->setLayout(lLight);
    pal.setColor(QPalette::Window, Qt::white);
    wLight->setAutoFillBackground(true);
    wLight->setPalette(pal);
    // Setup signals
    connect(editLightDiv,SIGNAL(textChanged(QString)),this,SLOT(lightDivChanged()));

    // Soil tab -----------------------------------------------------------------------------------------
    chartSoil = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartSoil->setTitle((tr("Soilerature")));
    chartSoil->legend()->hide();
    chartSoil->setAnimationOptions(QChart::AllAnimations);
    chartSoil->setAxisYLabel(tr("H_s [%]"));
    viewSoil = new QChartView(chartSoil);
    viewSoil->setRenderHint(QPainter::Antialiasing);
    //viewSoil->setSceneRect(QRectF(0,0,700,400));
    viewSoil->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblSoil = new QLabel(tr("Soil humidity: "));
    lblSoil->setStyleSheet(styleFont);
    lblSoilValue = new QLabel(tr("0°C"));
    lblSoilValue->setStyleSheet(styleFont);
    lblSoilDiv = new QLabel(tr("t_div: "));
    lblSoilDiv->setStyleSheet(styleFont);
    editSoilDiv = new QLineEdit;
    editSoilDiv->setFixedWidth(100);
    editSoilDiv->setFixedHeight(30);
    editSoilDiv->setText("1");
    editSoilDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblSoilDivValue = new QLabel(tr("hour"));
    lblSoilDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblSoilDaySet = new QLabel(tr("Soil humidity set at: "));
    lblSoilDaySet->setStyleSheet(styleFont);
    lblSoilDaySetValue = new QLabel(tr("0°C"));
    lblSoilDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblSoilNightSet = new QLabel(tr("Soil humidity at: "));
    lblSoilNightSet->setStyleSheet(styleFont);
    lblSoilNightSetValue = new QLabel(tr("0°C"));
    lblSoilNightSetValue->setStyleSheet(styleFont);
    // Setup Soil widget, layout
    lSoil = new QHBoxLayout;
    lSoilLabels = new QVBoxLayout;
    lSoilLblValue = new QHBoxLayout;            // Actual value
    lSoilLblValue->addWidget(lblSoil);
    lSoilLblValue->addWidget(lblSoilValue);
    lSoilLabels->addLayout(lSoilLblValue);
    lSoilLblDiv = new QHBoxLayout;              // Shown history
    lSoilLblDiv->addWidget(lblSoilDiv);
    lSoilLblDiv->addWidget(editSoilDiv);
    lSoilLblDiv->addWidget(lblSoilDivValue);
    lSoilLabels->addLayout(lSoilLblDiv);
    // Day frame
    fSoilDay = new QFrame;
    lSoilDayLabels = new QVBoxLayout;
    lSoilDayLblSet = new QHBoxLayout;
    lSoilDayLabels->addWidget(lblTempDay);
    lSoilDayLblSet->addWidget(lblSoilDaySet);         // Wanted value
    lSoilDayLblSet->addWidget(lblSoilDaySetValue);
    lSoilDayLabels->addLayout(lSoilDayLblSet);      // Put labels into labels layout
    fSoilDay->setLayout(lSoilDayLabels);
    lSoilLabels->addWidget(fSoilDay);
    // Night frame
    fSoilNight = new QFrame;
    lSoilNightLabels = new QVBoxLayout;
    lSoilNightLblSet = new QHBoxLayout;
    lSoilNightLabels->addWidget(lblTempNight);
    lSoilNightLblSet->addWidget(lblSoilNightSet);         // Wanted value
    lSoilNightLblSet->addWidget(lblSoilNightSetValue);
    lSoilNightLabels->addLayout(lSoilNightLblSet);      // Put labels into labels layout
    fSoilNight->setLayout(lSoilNightLabels);
    lSoilLabels->addWidget(fSoilNight);
    // Setup layouts
    fSoilDay->setFrameShape(QFrame::WinPanel);
    fSoilDay->setBackgroundRole(QPalette::Shadow);
    fSoilDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fSoilDay->setAutoFillBackground(true);
    fSoilDay->setPalette(pal);
    fSoilNight->setFrameShape(QFrame::WinPanel);
    fSoilNight->setBackgroundRole(QPalette::Shadow);
    fSoilNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fSoilNight->setAutoFillBackground(true);
    fSoilNight->setPalette(pal);
    lSoil->addWidget(viewSoil);            // Put chart with labels into main H layout
    lSoil->addLayout(lSoilLabels);
    wSoil = new QWidget;
    wSoil->setLayout(lSoil);
    pal.setColor(QPalette::Window, Qt::white);
    wSoil->setAutoFillBackground(true);
    wSoil->setPalette(pal);
    // Setup signals
    connect(editSoilDiv,SIGNAL(textChanged(QString)),this,SLOT(soilDivChanged()));

    // Watter cup tab -----------------------------------------------------------------------------------------
    chartCup = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartCup->setTitle((tr("Cuperature")));
    chartCup->legend()->hide();
    chartCup->setAnimationOptions(QChart::AllAnimations);
    chartCup->setAxisYLabel(tr("L_c [%]"));
    viewCup = new QChartView(chartCup);
    viewCup->setRenderHint(QPainter::Antialiasing);
    //viewCup->setSceneRect(QRectF(0,0,700,400));
    viewCup->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblCup = new QLabel(tr("Cup watter level: "));
    lblCup->setStyleSheet(styleFont);
    lblCupValue = new QLabel(tr("0°C"));
    lblCupValue->setStyleSheet(styleFont);
    lblCupDiv = new QLabel(tr("t_div: "));
    lblCupDiv->setStyleSheet(styleFont);
    editCupDiv = new QLineEdit;
    editCupDiv->setFixedWidth(100);
    editCupDiv->setFixedHeight(30);
    editCupDiv->setText("1");
    editCupDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblCupDivValue = new QLabel(tr("hour"));
    lblCupDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblCupDaySet = new QLabel(tr("Cup level set at: "));
    lblCupDaySet->setStyleSheet(styleFont);
    lblCupDaySetValue = new QLabel(tr("0°C"));
    lblCupDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblCupNightSet = new QLabel(tr("Cup level set at: "));
    lblCupNightSet->setStyleSheet(styleFont);
    lblCupNightSetValue = new QLabel(tr("0°C"));
    lblCupNightSetValue->setStyleSheet(styleFont);
    // Setup Cup widget, layout
    lCup = new QHBoxLayout;
    lCupLabels = new QVBoxLayout;
    lCupLblValue = new QHBoxLayout;            // Actual value
    lCupLblValue->addWidget(lblCup);
    lCupLblValue->addWidget(lblCupValue);
    lCupLabels->addLayout(lCupLblValue);
    lCupLblDiv = new QHBoxLayout;              // Shown history
    lCupLblDiv->addWidget(lblCupDiv);
    lCupLblDiv->addWidget(editCupDiv);
    lCupLblDiv->addWidget(lblCupDivValue);
    lCupLabels->addLayout(lCupLblDiv);
    // Day frame
    fCupDay = new QFrame;
    lCupDayLabels = new QVBoxLayout;
    lCupDayLblSet = new QHBoxLayout;
    lCupDayLabels->addWidget(lblTempDay);
    lCupDayLblSet->addWidget(lblCupDaySet);         // Wanted value
    lCupDayLblSet->addWidget(lblCupDaySetValue);
    lCupDayLabels->addLayout(lCupDayLblSet);      // Put labels into labels layout
    fCupDay->setLayout(lCupDayLabels);
    lCupLabels->addWidget(fCupDay);
    // Night frame
    fCupNight = new QFrame;
    lCupNightLabels = new QVBoxLayout;
    lCupNightLblSet = new QHBoxLayout;
    lCupNightLabels->addWidget(lblTempNight);
    lCupNightLblSet->addWidget(lblCupNightSet);         // Wanted value
    lCupNightLblSet->addWidget(lblCupNightSetValue);
    lCupNightLabels->addLayout(lCupNightLblSet);      // Put labels into labels layout
    fCupNight->setLayout(lCupNightLabels);
    lCupLabels->addWidget(fCupNight);
    // Setup layouts
    fCupDay->setFrameShape(QFrame::WinPanel);
    fCupDay->setBackgroundRole(QPalette::Shadow);
    fCupDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fCupDay->setAutoFillBackground(true);
    fCupDay->setPalette(pal);
    fCupNight->setFrameShape(QFrame::WinPanel);
    fCupNight->setBackgroundRole(QPalette::Shadow);
    fCupNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fCupNight->setAutoFillBackground(true);
    fCupNight->setPalette(pal);
    lCup->addWidget(viewCup);            // Put chart with labels into main H layout
    lCup->addLayout(lCupLabels);
    wCup = new QWidget;
    wCup->setLayout(lCup);
    pal.setColor(QPalette::Window, Qt::white);
    wCup->setAutoFillBackground(true);
    wCup->setPalette(pal);
    // Setup signals
    connect(editCupDiv,SIGNAL(textChanged(QString)),this,SLOT(cupDivChanged()));

    // Watter rez tab -----------------------------------------------------------------------------------------
    chartRez = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartRez->setTitle((tr("Rezerature")));
    chartRez->legend()->hide();
    chartRez->setAnimationOptions(QChart::AllAnimations);
    chartRez->setAxisYLabel(tr("L_r [%]"));
    viewRez = new QChartView(chartRez);
    viewRez->setRenderHint(QPainter::Antialiasing);
    //viewRez->setSceneRect(QRectF(0,0,700,400));
    viewRez->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblRez = new QLabel(tr("Rez. watter level: "));
    lblRez->setStyleSheet(styleFont);
    lblRezValue = new QLabel(tr("0°C"));
    lblRezValue->setStyleSheet(styleFont);
    lblRezDiv = new QLabel(tr("t_div: "));
    lblRezDiv->setStyleSheet(styleFont);
    editRezDiv = new QLineEdit;
    editRezDiv->setFixedWidth(100);
    editRezDiv->setFixedHeight(30);
    editRezDiv->setText("1");
    editRezDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblRezDivValue = new QLabel(tr("hour"));
    lblRezDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblRezDaySet = new QLabel(tr("Rez. level set at: "));
    lblRezDaySet->setStyleSheet(styleFont);
    lblRezDaySetValue = new QLabel(tr("0°C"));
    lblRezDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblRezNightSet = new QLabel(tr("Rez. level set at: "));
    lblRezNightSet->setStyleSheet(styleFont);
    lblRezNightSetValue = new QLabel(tr("0°C"));
    lblRezNightSetValue->setStyleSheet(styleFont);
    // Setup Rez widget, layout
    lRez = new QHBoxLayout;
    lRezLabels = new QVBoxLayout;
    lRezLblValue = new QHBoxLayout;            // Actual value
    lRezLblValue->addWidget(lblRez);
    lRezLblValue->addWidget(lblRezValue);
    lRezLabels->addLayout(lRezLblValue);
    lRezLblDiv = new QHBoxLayout;              // Shown history
    lRezLblDiv->addWidget(lblRezDiv);
    lRezLblDiv->addWidget(editRezDiv);
    lRezLblDiv->addWidget(lblRezDivValue);
    lRezLabels->addLayout(lRezLblDiv);
    // Day frame
    fRezDay = new QFrame;
    lRezDayLabels = new QVBoxLayout;
    lRezDayLblSet = new QHBoxLayout;
    lRezDayLabels->addWidget(lblTempDay);
    lRezDayLblSet->addWidget(lblRezDaySet);         // Wanted value
    lRezDayLblSet->addWidget(lblRezDaySetValue);
    lRezDayLabels->addLayout(lRezDayLblSet);      // Put labels into labels layout
    fRezDay->setLayout(lRezDayLabels);
    //lRezLabels->addWidget(fRezDay);
    // Night frame
    fRezNight = new QFrame;
    lRezNightLabels = new QVBoxLayout;
    lRezNightLblSet = new QHBoxLayout;
    lRezNightLabels->addWidget(lblTempNight);
    lRezNightLblSet->addWidget(lblRezNightSet);         // Wanted value
    lRezNightLblSet->addWidget(lblRezNightSetValue);
    lRezNightLabels->addLayout(lRezNightLblSet);      // Put labels into labels layout
    fRezNight->setLayout(lRezNightLabels);
    //lRezLabels->addWidget(fRezNight);
    // Setup layouts
    fRezDay->setFrameShape(QFrame::WinPanel);
    fRezDay->setBackgroundRole(QPalette::Shadow);
    fRezDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fRezDay->setAutoFillBackground(true);
    fRezDay->setPalette(pal);
    fRezNight->setFrameShape(QFrame::WinPanel);
    fRezNight->setBackgroundRole(QPalette::Shadow);
    fRezNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fRezNight->setAutoFillBackground(true);
    fRezNight->setPalette(pal);
    lRez->addWidget(viewRez);            // Put chart with labels into main H layout
    lRez->addLayout(lRezLabels);
    wRez = new QWidget;
    wRez->setLayout(lRez);
    pal.setColor(QPalette::Window, Qt::white);
    wRez->setAutoFillBackground(true);
    wRez->setPalette(pal);
    // Setup signals
    connect(editRezDiv,SIGNAL(textChanged(QString)),this,SLOT(rezDivChanged()));

    // Test rand data tab -----------------------------------------------------------------------------------------
    chartTest = new Chart;
    styleFont = "color: black; font-size: 20px";
    //chartTest->setTitle((tr("Testerature")));
    chartTest->legend()->hide();
    chartTest->setAnimationOptions(QChart::AllAnimations);
    chartTest->setAxisYLabel(tr("L_r [%]"));
    viewTest = new QChartView(chartTest);
    viewTest->setRenderHint(QPainter::Antialiasing);
    //viewTest->setSceneRect(QRectF(0,0,700,400));
    viewTest->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    lblTest = new QLabel(tr("Testidity: "));
    lblTest->setStyleSheet(styleFont);
    lblTestValue = new QLabel(tr("0°C"));
    lblTestValue->setStyleSheet(styleFont);
    lblTestDiv = new QLabel(tr("t_div: "));
    lblTestDiv->setStyleSheet(styleFont);
    editTestDiv = new QLineEdit;
    editTestDiv->setFixedWidth(100);
    editTestDiv->setFixedHeight(30);
    editTestDiv->setText("1");
    editTestDiv->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    lblTestDivValue = new QLabel(tr("hour"));
    lblTestDivValue->setStyleSheet(styleFont);
    // Day frame
    lblTempDay = new QLabel(tr("Day:"));
    lblTempDay->setStyleSheet("color: black; font: bold 20px");
    lblTestDaySet = new QLabel(tr("Testidity set at: "));
    lblTestDaySet->setStyleSheet(styleFont);
    lblTestDaySetValue = new QLabel(tr("0°C"));
    lblTestDaySetValue->setStyleSheet(styleFont);
    // Night frame
    lblTempNight = new QLabel(tr("Night"));
    lblTempNight->setStyleSheet("color: black; font: bold 20px");
    lblTestNightSet = new QLabel(tr("Testidity set at: "));
    lblTestNightSet->setStyleSheet(styleFont);
    lblTestNightSetValue = new QLabel(tr("0°C"));
    lblTestNightSetValue->setStyleSheet(styleFont);
    // Setup Test widget, layout
    lTest = new QHBoxLayout;
    lTestLabels = new QVBoxLayout;
    lTestLblValue = new QHBoxLayout;            // Actual value
    lTestLblValue->addWidget(lblTest);
    lTestLblValue->addWidget(lblTestValue);
    lTestLabels->addLayout(lTestLblValue);
    lTestLblDiv = new QHBoxLayout;              // Shown history
    lTestLblDiv->addWidget(lblTestDiv);
    lTestLblDiv->addWidget(editTestDiv);
    lTestLblDiv->addWidget(lblTestDivValue);
    lTestLabels->addLayout(lTestLblDiv);
    // Day frame
    fTestDay = new QFrame;
    lTestDayLabels = new QVBoxLayout;
    lTestDayLblSet = new QHBoxLayout;
    lTestDayLabels->addWidget(lblTempDay);
    lTestDayLblSet->addWidget(lblTestDaySet);         // Wanted value
    lTestDayLblSet->addWidget(lblTestDaySetValue);
    lTestDayLabels->addLayout(lTestDayLblSet);      // Put labels into labels layout
    fTestDay->setLayout(lTestDayLabels);
    lTestLabels->addWidget(fTestDay);
    // Night frame
    fTestNight = new QFrame;
    lTestNightLabels = new QVBoxLayout;
    lTestNightLblSet = new QHBoxLayout;
    lTestNightLabels->addWidget(lblTempNight);
    lTestNightLblSet->addWidget(lblTestNightSet);         // Wanted value
    lTestNightLblSet->addWidget(lblTestNightSetValue);
    lTestNightLabels->addLayout(lTestNightLblSet);      // Put labels into labels layout
    fTestNight->setLayout(lTestNightLabels);
    lTestLabels->addWidget(fTestNight);
    // Setup layouts
    fTestDay->setFrameShape(QFrame::WinPanel);
    fTestDay->setBackgroundRole(QPalette::Shadow);
    fTestDay->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fTestDay->setAutoFillBackground(true);
    fTestDay->setPalette(pal);
    fTestNight->setFrameShape(QFrame::WinPanel);
    fTestNight->setBackgroundRole(QPalette::Shadow);
    fTestNight->setFrameShadow(QFrame::Raised);
    pal.setColor(QPalette::Shadow, Qt::gray);
    fTestNight->setAutoFillBackground(true);
    fTestNight->setPalette(pal);
    lTest->addWidget(viewTest);            // Put chart with labels into main H layout
    lTest->addLayout(lTestLabels);
    wTest = new QWidget;
    wTest->setLayout(lTest);
    pal.setColor(QPalette::Window, Qt::white);
    wTest->setAutoFillBackground(true);
    wTest->setPalette(pal);
    // Setup signals
    connect(editTestDiv,SIGNAL(textChanged(QString)),this,SLOT(testDivChanged()));
}

void MainWindow::tempDivChanged(){
    if(editTempDiv->text().toInt() > 0){
        chartTemp->setXRange(editTempDiv->text().toInt());
       //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editTempDiv->text().toInt()),2000);
    }else{
        chartTemp->setXRange(1);
        editTempDiv->setText("1");
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::humDivChanged(){
    if(editHumDiv->text().toInt() > 0){
        chartHum->setXRange(editHumDiv->text().toInt());
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editHumDiv->text().toInt()),2000);
    }else{
        chartHum->setXRange(1);
        editHumDiv->setText("1");
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::lightDivChanged(){
    if(editLightDiv->text().toInt() > 0){
        chartLight->setXRange(editLightDiv->text().toInt());
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editLightDiv->text().toInt()),2000);
    }else{
        chartLight->setXRange(1);
        editLightDiv->setText("1");
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::soilDivChanged(){
    if(editSoilDiv->text().toInt() > 0){
        chartSoil->setXRange(editSoilDiv->text().toInt());
       // this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editSoilDiv->text().toInt()),2000);
    }else{
        chartSoil->setXRange(1);
        editSoilDiv->setText("1");
       // this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::cupDivChanged(){
    if(editCupDiv->text().toInt() > 0){
        chartCup->setXRange(editCupDiv->text().toInt());
       // this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editCupDiv->text().toInt()),2000);
    }else{
        chartCup->setXRange(1);
        editCupDiv->setText("1");
      //  this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::rezDivChanged(){
    if(editRezDiv->text().toInt() > 0){
        chartRez->setXRange(editRezDiv->text().toInt());
       // this->statusBar()->showMessage(tr("Div changed to ")+QString::number(editRezDiv->text().toInt()),2000);
    }else{
        chartRez->setXRange(1);
        editRezDiv->setText("1");
        //this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
    }
}

void MainWindow::testDivChanged(){
    bool ok;
    if(!editTestDiv->text().isEmpty()){
        qreal number = editTestDiv->text().toDouble(&ok);
        if(number > 0.0 && ok == true){
            chartTest->setXRange(number);
            this->statusBar()->showMessage(tr("Div changed to ")+QString::number(number),2000);
        }else{
            chartTest->setXRange(1);
            this->statusBar()->showMessage(tr("Div changed to ")+QString::number(1),2000);
        }
    }
}

void MainWindow::confPot(){
    configPot *myConfig = new configPot;
    myConfig->setMode(mode);
    myConfig->setHum(setHum);
    myConfig->setTemp(setTemp);
    myConfig->setKeepTemp(keepTemp);
    myConfig->exec();
    if(myConfig->saved){
        if(myConfig->getKeepTemp() == 1){
            keepTemp = true;
        }else{
            keepTemp = false;
        }
        setTemp = myConfig->getTemp();
        mode = myConfig->getMode();
        setHum = myConfig->getHum();
    }
    printSetValues();
    iot_client->sendData((uint8_t) mode, (uint8_t) setTemp,(uint8_t)setHum);
}


void MainWindow::setupMQTT(){
    int rc;
    char client_id[] = "flowerpot-arduino@ttn";
    char host[] = "eu1.cloud.thethings.network";
    int port = 1883;
    char password[] = "NNSXS.VHDQHH5G3AGSELLAO3AVDGOV443LKB2JP2FZCCY.NZ5SBYUGLYQGH5UXNM2TXUSBI2R2HAIZJFATVQHFGLYTEYICSTCQ";
    mosqpp::lib_init();

    iot_client = new mqtt_client(client_id, host, port);
    rc = iot_client->username_pw_set(client_id,password);
    if(rc == MOSQ_ERR_SUCCESS){
        qDebug()<<"PW OK";
    }
    if(iot_client->loop_start() == MOSQ_ERR_SUCCESS)qDebug()<<"Loop started";

    connect(iot_client->e,SIGNAL(receivedData(QList<int>)),this,SLOT(loraReceivedData(QList<int>)));
    connect(iot_client->e,SIGNAL(disconnected()),this,SLOT(loraDisconnected()));
    connect(iot_client->e,SIGNAL(connected()),this,SLOT(loraConnected()));

    // Reconnection timer
    timerReconnection = new QTimer;
    timerReconnection->setInterval(5000);
    connect(timerReconnection, SIGNAL(timeout()), this, SLOT(loraDisconnected()));
    timerReconnection->start(); // reconnect every interval until connected
}

void MainWindow::loraDisconnected(){
    this->statusBar()->showMessage(tr("Server disconnected!"),10000);
    iot_client->reconnect();
    if(!timerReconnection->isActive()){
        timerReconnection->start();
    }
}

void MainWindow::loraConnected(){
    this->statusBar()->showMessage(tr("Connected to server!"),10000);
    if(timerReconnection->isActive()){
        timerReconnection->stop();
    }
    char topic[] = "#";
    iot_client->subscribe(NULL, topic);
    timerTest->start();
}

/*
 * Grabs received and translated data packet and prints it
 * data: {t,h,light,soil,cup,rez};
 */
void MainWindow::loraReceivedData(QList<int> data){
    this->statusBar()->showMessage(tr("Received data from device!"),2);
    printStats(data[0],data[1],data[2],data[3],data[4],data[5]);
    printTemp(data[0]);
    printHum(data[1]);
    printLight(data[2]);
    printSoil(data[3]);
    printCup(data[4]);
    printRez(data[5]);
    if(data.size() != 6){
        // mode set
        if(data[6] != mode ||
          (data[7] == 0 && keepTemp == true)||
          (data[7] != 0 && keepTemp == false)||
          (data[7] != (uint8_t)setTemp && keepTemp == true )||
          (data[8] != (uint8_t)setHum)){
            this->statusBar()->showMessage(tr("Overwriting the data inside device!"),2);
            iot_client->sendData((uint8_t) mode, (uint8_t) setTemp,(uint8_t)setHum);
        }
    }
    /*    bool keepTemp;
    qreal setTemp;
    qreal setHum;
    int mode;*/

    qDebug()<<data;
}

void MainWindow::test(){
    qreal data = distribution(this->generator);
    uint16_t c = std::round(data);
    char TXBuffer[64];
    *((uint16_t *)TXBuffer) = c;
    qDebug()<<iot_client->publish(NULL,PUBLISH_TOPIC,2,TXBuffer);
    iot_client->sendNumber(1500);
    //qDebug()<<MOSQ_ERR_SUCCESS;
    qDebug()<<"Manudal data sent";
    printTest(data);

}

void MainWindow::printTemp(qreal value){
    lblTempValue->setText(QString::number(value)+="°C");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartTemp->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString("Current time [hours]: ")+QString::number(time);
    qDebug()<<QString("Current temperature [°C]: ")+QString::number(value);
#endif
}

void MainWindow::printHum(qreal value){
    lblHumValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    //time = QDateTime::currentDateTime().time().minute();//zobrazuji minuty
    //time += QDateTime::currentDateTime().time().second()/60;
    //time = QDateTime::currentDateTime().time().second()/10.0;//zobrazuji desítky sekund
    //time = time /5;//zobarzuji pět minut
    chartHum->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString().asprintf("Current time [hours]: %0.2f", time);
    qDebug()<<QString().asprintf("Current humidity [%]: %0.2f", value);
    qDebug()<<" ";
#endif
}

void MainWindow::printLight(qreal value){
    lblLightValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartLight->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString("Current time [hours]: ")+QString::number(time);
    qDebug()<<QString("Current light [%]: ")+QString::number(value);
#endif
}

void MainWindow::printSoil(qreal value){
    lblSoilValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartSoil->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString("Current time [hours]: ")+QString::number(time);
    qDebug()<<QString("Current soil humidity [%]: ")+QString::number(value);
#endif
}

void MainWindow::printCup(qreal value){
    lblCupValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartCup->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString("Current time [hours]: ")+QString::number(time);
    qDebug()<<QString("Current Cup humidity [%]: ")+QString::number(value);
#endif
}

void MainWindow::printRez(qreal value){
    lblRezValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartRez->addPoint(time,value);
#ifdef QT_DEBUG
    qDebug()<<QString("Current time [hours]: ")+QString::number(time);
    qDebug()<<QString("Current Rez humidity [%]: ")+QString::number(value);
#endif
}

void MainWindow::printTest(qreal value){
    lblTestValue->setText(QString::number(value)+="%");
    qreal time = QDateTime::currentDateTime().time().hour();    // čas musí být v hodinách
    time += QDateTime::currentDateTime().time().minute()/60.0;
    time += QDateTime::currentDateTime().time().second()/3600.0;
    chartTest->addPoint(time,value);
}

/*
 * Vykreslení dat v Tabu stats
 */
void MainWindow::printStats(qreal temp, qreal hum, qreal light, qreal soil, qreal cup, qreal rez){
    // Dials and current values
    viewDialTemp->rootObject()->setProperty("value",temp);
    lblStatsTempValue->setText(QString::number(temp)+="°C");
    viewDialHum->rootObject()->setProperty("value",hum);
    lblStatsHumValue->setText(QString::number(hum)+="%");
    viewDialLight->rootObject()->setProperty("value",light);
    lblStatsLightValue->setText(QString::number(light)+="%");
    viewDialLight->rootObject()->setProperty("value",soil);
    lblStatsLightValue->setText(QString::number(soil)+="%");
    viewDialLight->rootObject()->setProperty("value",cup);
    lblStatsLightValue->setText(QString::number(cup)+="%");
    viewDialLight->rootObject()->setProperty("value",rez);
    lblStatsLightValue->setText(QString::number(rez)+="%");
    progStatsWatterCup->setValue(std::round(cup));
    lblStatsWatterCupValue->setText(QString::number(rez)+="%");
    progStatsWatterRez->setValue(std::round(rez));
    lblStatsWatterRezValue->setText(QString::number(rez)+="%");
}



/*
void MainWindow::test(){
    //std::string host = "eu1.cloud.thethings.network";
    //std::string port = "8883";
    //std::string cacert = "NNSXS.NENL24QE4UFPUEWIOASOQNIXTJILUEC2CHEK3NI.A45SR65A5M4NUVSSHKQTASDFZSZVS4ZL6YOMTONHLAGDEXURFOQQ";
    //std::string id = "1";

    class mqtt_client *iot_client;
    int rc;

    char client_id[] = "flowerpot-arduino@ttn";
    char host[] = "eu1.cloud.thethings.network";
    int port = 1883;
    char topic[] = "#";
    char password[] = "NNSXS.VHDQHH5G3AGSELLAO3AVDGOV443LKB2JP2FZCCY.NZ5SBYUGLYQGH5UXNM2TXUSBI2R2HAIZJFATVQHFGLYTEYICSTCQ";

    mosqpp::lib_init();

    iot_client = new mqtt_client(client_id, host, port);

    rc = iot_client->username_pw_set(client_id,password);
    if(rc == MOSQ_ERR_SUCCESS){
        qDebug()<<"PW OK";
    }

    while(1)
    {
        QCoreApplication::processEvents();
        //printLight(5);
        rc = iot_client->loop();
        if (rc)
        {
            //qDebug()<<QString::number(rc);
            iot_client->reconnect();
        }
        else{
            iot_client->subscribe(NULL, topic);

            //if(iot_client->h!=-1){
            //    printHum(iot_client->h);
            //    iot_client->h = -1;
            //}
        }

    }
    //MOSQ_ERR_SUCCESS
    mosqpp::lib_cleanup();
}
*/







MainWindow::~MainWindow()
{
    mosqpp::lib_cleanup();
}

