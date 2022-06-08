#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configpot.h"
#include <QMainWindow>
#include <random>

class mqtt_client;
class Chart;
class configPot;
class calibrate;

class QChartView;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QScrollArea;
class QTimer;
class QMenu;
class QAction;
class QToolBar;
class QQuickView;
class QTabWidget;
class QProgressBar;
class QFrame;
class QLineEdit;
class QSerialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void setupForm();
    void setupMQTT();
    void setupActions();
    void setupTabs();

    mqtt_client *iot_client;

    // Stats Tab
    QWidget *wStats;
    QVBoxLayout *lStatsTemp;
    QQuickView *viewDialTemp;
    QWidget *contDialTemp;
    QVBoxLayout *lStatsHum;
    QQuickView *viewDialHum;
    QWidget *contDialHum;
    QVBoxLayout *lStatsSoil;
    QQuickView *viewDialSoil;
    QWidget *contDialSoil;
    QVBoxLayout *lStatsLight;
    QQuickView *viewDialLight;
    QWidget *contDialLight;
    QProgressBar *progStatsWatterCup;
    QVBoxLayout *lStatsWatterCup;
    QProgressBar *progStatsWatterRez;
    QVBoxLayout *lStatsWatterRez;
    QHBoxLayout *lStatsDials;
    QLabel *lblStatsTemp;
    QLabel *lblStatsTempValue;
    QLabel *lblStatsHum;
    QLabel *lblStatsHumValue;
    QLabel *lblStatsLight;
    QLabel *lblStatsLightValue;
    QLabel *lblStatsSoil;
    QLabel *lblStatsSoilValue;
    QLabel *lblStatsWatterCup;
    QLabel *lblStatsWatterCupValue;
    QLabel *lblStatsWatterRez;
    QLabel *lblStatsWatterRezValue;

    // Temperature Tab
    QWidget *wTemp;             // Main widget
    Chart *chartTemp;           // Chart
    QChartView *viewTemp;
    QHBoxLayout *lTempLblDiv;
    QLabel *lblTempDiv;
    QLineEdit *editTempDiv;
    QLabel *lblTempDivValue;
    QHBoxLayout *lTemp;         // Labels
    QVBoxLayout *lTempLabels;
    QHBoxLayout *lTempLblValue;
    QLabel *lblTemp;
    QLabel *lblTempValue;
    QFrame *fTempDay;           // Day frame
    QLabel *lblTempDay;
    QVBoxLayout *lTempDayLabels;
    QHBoxLayout *lTempDayLblSet;
    QLabel *lblTempDaySet;
    QLabel *lblTempDaySetValue;
    QFrame *fTempNight;         // Night frame
    QLabel *lblTempNight;
    QVBoxLayout *lTempNightLabels;
    QHBoxLayout *lTempNightLblSet;
    QLabel *lblTempNightSet;
    QLabel *lblTempNightSetValue;

    // Humidity Tab
    QWidget *wHum;             // Main widget
    Chart *chartHum;           // Chart
    QChartView *viewHum;
    QHBoxLayout *lHumLblDiv;
    QLabel *lblHumDiv;
    QLineEdit *editHumDiv;
    QLabel *lblHumDivValue;
    QHBoxLayout *lHum;         // Labels
    QVBoxLayout *lHumLabels;
    QHBoxLayout *lHumLblValue;
    QLabel *lblHum;
    QLabel *lblHumValue;
    QFrame *fHumDay;           // Day frame
    QLabel *lblHumDay;
    QVBoxLayout *lHumDayLabels;
    QHBoxLayout *lHumDayLblSet;
    QLabel *lblHumDaySet;
    QLabel *lblHumDaySetValue;
    QFrame *fHumNight;         // Night frame
    QLabel *lblHumNight;
    QVBoxLayout *lHumNightLabels;
    QHBoxLayout *lHumNightLblSet;
    QLabel *lblHumNightSet;
    QLabel *lblHumNightSetValue;

    // Light Tab
    QWidget *wLight;             // Main widget
    Chart *chartLight;           // Chart
    QChartView *viewLight;
    QHBoxLayout *lLightLblDiv;
    QLabel *lblLightDiv;
    QLineEdit *editLightDiv;
    QLabel *lblLightDivValue;
    QHBoxLayout *lLight;         // Labels
    QVBoxLayout *lLightLabels;
    QHBoxLayout *lLightLblValue;
    QLabel *lblLight;
    QLabel *lblLightValue;
    QFrame *fLightDay;           // Day frame
    QLabel *lblLightDay;
    QVBoxLayout *lLightDayLabels;
    QHBoxLayout *lLightDayLblSet;
    QLabel *lblLightDaySet;
    QLabel *lblLightDaySetValue;
    QFrame *fLightNight;         // Night frame
    QLabel *lblLightNight;
    QVBoxLayout *lLightNightLabels;
    QHBoxLayout *lLightNightLblSet;
    QLabel *lblLightNightSet;
    QLabel *lblLightNightSetValue;

    // Soil Tab
    QWidget *wSoil;             // Main widget
    Chart *chartSoil;           // Chart
    QChartView *viewSoil;
    QHBoxLayout *lSoilLblDiv;
    QLabel *lblSoilDiv;
    QLineEdit *editSoilDiv;
    QLabel *lblSoilDivValue;
    QHBoxLayout *lSoil;         // Labels
    QVBoxLayout *lSoilLabels;
    QHBoxLayout *lSoilLblValue;
    QLabel *lblSoil;
    QLabel *lblSoilValue;
    QFrame *fSoilDay;           // Day frame
    QLabel *lblSoilDay;
    QVBoxLayout *lSoilDayLabels;
    QHBoxLayout *lSoilDayLblSet;
    QLabel *lblSoilDaySet;
    QLabel *lblSoilDaySetValue;
    QFrame *fSoilNight;         // Night frame
    QLabel *lblSoilNight;
    QVBoxLayout *lSoilNightLabels;
    QHBoxLayout *lSoilNightLblSet;
    QLabel *lblSoilNightSet;
    QLabel *lblSoilNightSetValue;

    // Watter Cup Tab
    QWidget *wCup;             // Main widget
    Chart *chartCup;           // Chart
    QChartView *viewCup;
    QHBoxLayout *lCupLblDiv;
    QLabel *lblCupDiv;
    QLineEdit *editCupDiv;
    QLabel *lblCupDivValue;
    QHBoxLayout *lCup;         // Labels
    QVBoxLayout *lCupLabels;
    QHBoxLayout *lCupLblValue;
    QLabel *lblCup;
    QLabel *lblCupValue;
    QFrame *fCupDay;           // Day frame
    QLabel *lblCupDay;
    QVBoxLayout *lCupDayLabels;
    QHBoxLayout *lCupDayLblSet;
    QLabel *lblCupDaySet;
    QLabel *lblCupDaySetValue;
    QFrame *fCupNight;         // Night frame
    QLabel *lblCupNight;
    QVBoxLayout *lCupNightLabels;
    QHBoxLayout *lCupNightLblSet;
    QLabel *lblCupNightSet;
    QLabel *lblCupNightSetValue;

    // Watter Rez Tab
    QWidget *wRez;             // Main widget
    Chart *chartRez;           // Chart
    QChartView *viewRez;
    QHBoxLayout *lRezLblDiv;
    QLabel *lblRezDiv;
    QLineEdit *editRezDiv;
    QLabel *lblRezDivValue;
    QHBoxLayout *lRez;         // Labels
    QVBoxLayout *lRezLabels;
    QHBoxLayout *lRezLblValue;
    QLabel *lblRez;
    QLabel *lblRezValue;
    QFrame *fRezDay;           // Day frame
    QLabel *lblRezDay;
    QVBoxLayout *lRezDayLabels;
    QHBoxLayout *lRezDayLblSet;
    QLabel *lblRezDaySet;
    QLabel *lblRezDaySetValue;
    QFrame *fRezNight;         // Night frame
    QLabel *lblRezNight;
    QVBoxLayout *lRezNightLabels;
    QHBoxLayout *lRezNightLblSet;
    QLabel *lblRezNightSet;
    QLabel *lblRezNightSetValue;

    // Test rand data Tab
    QWidget *wTest;             // Main widget
    Chart *chartTest;           // Chart
    QChartView *viewTest;
    QHBoxLayout *lTestLblDiv;
    QLabel *lblTestDiv;
    QLineEdit *editTestDiv;
    QLabel *lblTestDivValue;
    QHBoxLayout *lTest;         // Labels
    QVBoxLayout *lTestLabels;
    QHBoxLayout *lTestLblValue;
    QLabel *lblTest;
    QLabel *lblTestValue;
    QFrame *fTestDay;           // Day frame
    QLabel *lblTestDay;
    QVBoxLayout *lTestDayLabels;
    QHBoxLayout *lTestDayLblSet;
    QLabel *lblTestDaySet;
    QLabel *lblTestDaySetValue;
    QFrame *fTestNight;         // Night frame
    QLabel *lblTestNight;
    QVBoxLayout *lTestNightLabels;
    QHBoxLayout *lTestNightLblSet;
    QLabel *lblTestNightSet;
    QLabel *lblTestNightSetValue;


    // Menu
    QMenu *mnuConf;
    QAction *actPotConf;
    QAction *actSerConf;
    QAction *actLoraConf;
    QAction *actLoraConnect;
    QAction *actCalibrate;

    // Config
    bool config_saved;
    bool keepTemp;
    qreal setTemp;
    qreal setHum;
    int mode;
    bool zalevam;
    bool ohrivam;

    //Kalibrace
    calibrate *myCalibration;
    bool calibrated;
    float k0,k1;

    // Measured properties
    float temperature;
    float humidity;
    int watter_cup;
    int watter_rez;
    float soil1;
    float soil2;

    //Serial port
    QString RXBuffer;
    QStringList RXLines;
    QSerialPort *serial;
    bool serialConnected;
    void setupSerial();
    int baudRate;
    QString portName;
    bool sendData(QString str, QString dack, bool silent = false);
    bool getPacket(QString cmd, QList<float> *values, bool silent = false);
    QTimer *timerSerialReconnection;
    bool configSent;

    // Tabs
    QTabWidget *wTabs;

    // Toolbar
    void updateToolbar();
    QToolBar *bar;
    QLabel *led;
    QLabel *ledConnection;
    QLabel *ledZalevam;
    QLabel *ledOhrivam;
    int SIZE;
    QString blueSS;
    QString orangeSS;
    QString redSS;
    QString greenSS;

    QAction *actSerialConf;
    QAction *actSerialConnect;

    QScrollArea * areaMain;


    QVBoxLayout *lMain;


    void printStats(qreal temp, qreal hum, qreal light, qreal soil, qreal cup, qreal rez);
    void printTemp(qreal value);
    void printHum(qreal value);
    void printLight(qreal value);
    void printSoil(qreal value);
    void printCup(qreal value);
    void printRez(qreal value);
    void printTest(qreal value);
    void printSetValues();

    QTimer *timerReconnection;   // zkontroluje připojení každý časový interval


    //testovací proměnné
    //void test();
    QTimer *timerTest;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void confPot();
    void confSer();
    void calibrateTemp();

    //Calibration slot
    void getTemperature();
    //serial port
    void serialReconnect();
    void readData();

    // LoRa things
    void loraDisconnected();
    void loraConnected();
    void loraReceivedData(QList<int> data);

    // Chart x axis history
    void tempDivChanged();
    void humDivChanged();
    void lightDivChanged();
    void soilDivChanged();
    void cupDivChanged();
    void rezDivChanged();
    void testDivChanged();

    void test();
};
#endif // MAINWINDOW_H
