#ifndef EMITOR_H
#define EMITOR_H

#include <QObject>

class emitor : public QObject
{
    Q_OBJECT
public:
    explicit emitor(QObject *parent = nullptr);

    void emitData(QList<int> data){emit receivedData(data);}
    void emitDisconnect(){emit disconnected();}
    void emitConnected(){emit connected();}
signals:
    void receivedData(QList<int> data); //{t,h,light,soil}
    void disconnected();
    void connected();

};

#endif // EMITOR_H
