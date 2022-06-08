QT       += core gui quick qml serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calibrate.cpp \
    chart.cpp \
    configpot.cpp \
    confserial.cpp \
    emitor.cpp \
    main.cpp \
    mainwindow.cpp \
    mqtt_client.cpp

HEADERS += \
    calibrate.h \
    chart.h \
    configpot.h \
    confserial.h \
    emitor.h \
    mainwindow.h \
    mqtt_client.h \

unix|win32: LIBS += -lmosquittopp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT += charts


#QT += mqtt


#target.path = $$[QT_INSTALL_EXAMPLES]/charts/dynamicspline
#INSTALLS += target

RESOURCES += \
    icons.qrc \
    qml.qrc
