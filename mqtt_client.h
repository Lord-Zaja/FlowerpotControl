#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H


#include <mosquittopp.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "emitor.h"

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class mqtt_client : public mosqpp::mosquittopp
{
    char TXBuffer[64];
    int TXSize;

    std::string getMyData(std::string str_buffer);
    int dataExtract(std::string myData,std::string varName);
    int getHumidity(std::string myData);
    int getTemperature(std::string myData);
    int getSoil(std::string myData);
    int getLight(std::string myData);
    int getCup(std::string myData);
    int getRez(std::string myData);

public:
    mqtt_client (const char *id, const char *host, int port);
    ~mqtt_client();

    emitor *e;
    //void sendData(char str){&TXBuffer[0] = str;}
    void sendNumber(uint16_t number){
        *((uint16_t *)TXBuffer) = number;
        TXSize = 2;
        /*uint8_t mask = 0xFF;
        char lsb = */
        //QByteArray qb;
        //qb.setNum(number);
        //TXBuffer = ;

    }

    // pokud je temp = 0 pak neudržovat konstantní teplotu
    void sendData(uint8_t mode, uint8_t temp, uint8_t hum);

    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);

};
#endif // MQTT_CLIENT_H
