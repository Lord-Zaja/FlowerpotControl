#include "mqtt_client.h"
#include <mosquittopp.h>
#include <QDebug>
#include <iostream>

#define PUBLISH_TOPIC "v3/flowerpot-arduino@ttn/devices/eui-e0080e1010101010/down/push"

#define QT_DEBUG

mqtt_client::mqtt_client(const char *id, const char *host, int port) : mosquittopp(id)

{
    int keepalive = DEFAULT_KEEP_ALIVE;
    connect(host, port, keepalive);
    e = new emitor;
}

mqtt_client::~mqtt_client()
{
}

void mqtt_client::on_connect(int rc)
{
    e->emitConnected();
    if (!rc)
    {
        #ifdef QT_DEBUG
            std::cout << "Connected - code " << rc << std::endl;
        #endif
    }
}

void mqtt_client::on_disconnect(int rc)
{
    e->emitDisconnect();
    if (!rc)
    {
        #ifdef QT_DEBUG
            std::cout << "Disconnected - code " << rc << std::endl;
        #endif
    }
}

void mqtt_client::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    #ifdef QT_DEBUG
        //std::cout << "Subscription succeeded." << std::endl;
    #endif
}

/*
 * \brief Vrátí můj packet z bufferu typu string
 */
std::string mqtt_client::getMyData(std::string str_buffer){
    int pre_idx  = str_buffer.find("decoded_payload");
    int start_idx = str_buffer.find("{",pre_idx);
    int end_idx = str_buffer.find("}",start_idx);
    std::string data = str_buffer.substr(start_idx,end_idx-start_idx);
    return data;
}

/*
 * Extract our data from packet
 */
int mqtt_client::dataExtract(std::string myData, std::string varName){
    std::string search = "\""+varName+"\":";
    int start_idx  = myData.find(search)+3+varName.size();
    int end_idx = myData.find(",",start_idx);
    if(end_idx == std::string::npos){   // na konci dat .. nenašlo čárku za číslem
        end_idx = myData.find("}",start_idx);
    }
    //qDebug()<<QString::fromStdString(myData.substr(start_idx,end_idx-start_idx));
    return std::stoi(myData.substr(start_idx,end_idx-start_idx));
}

/*
 * Get Humidity from our data packet //":{"adc1":781,"adc2":142,"h":32.8,"t":23.9
 */
int mqtt_client::getHumidity(std::string myData){
    return dataExtract(myData,"h");
}

/*
 * Get Temperature from our data packet //":{"adc1":781,"adc2":142,"h":32.8,"t":23.9
 */
int mqtt_client::getTemperature(std::string myData){
    return dataExtract(myData,"t");
}

/*
 * Get Temperature from our data packet //":{"adc1":781,"adc2":142,"h":32.8,"t":23.9
 */
int mqtt_client::getLight(std::string myData){
    return dataExtract(myData,"adc2")*100/1024;
}

/*
 * Get Temperature from our data packet //":{"adc1":781,"adc2":142,"h":32.8,"t":23.9
 */
int mqtt_client::getSoil(std::string myData){
    qreal voltage = dataExtract(myData,"adc1")*100/1024;
    // 0.4V == 100%
    // 2V == 0%
    return voltage;
}

/*
 * Get Cup watter level from our data packet: "{\"adc1\":89,\"adc2\":115,\"cup\":3,\"h\":40.8,\"res\":1,\"t\":26.4"
 */
int mqtt_client::getCup(std::string myData){
    int data = dataExtract(myData,"cup");
    if(data == 0)return 100;
    if(data == 1)return -1;
    if(data == 2)return 50;
    if(data == 3)return 1;
    return -1;
}

/*
 * Get Rez watter level from our data packet: "{\"adc1\":89,\"adc2\":115,\"cup\":3,\"h\":40.8,\"res\":1,\"t\":26.4"
 */
int mqtt_client::getRez(std::string myData){
    int data = dataExtract(myData,"res");
    if(data == 0)return 100;
    if(data == 1)return -1;
    if(data == 2)return 50;
    if(data == 3)return 1;
    return -1;
}

void mqtt_client::sendData(uint8_t mode, uint8_t temp, uint8_t hum){
    TXBuffer[0] = mode;
    TXBuffer[1] = temp;
    TXBuffer[2] = hum;
    TXSize = 3;
    //strlen(TXBuffer)
    //snprintf(buf, payload_size, "Turning on...");
    qDebug()<<publish(NULL, PUBLISH_TOPIC, TXSize, TXBuffer);
    #ifdef QT_DEBUG
        std::cout << "Data sent" << std::endl;
    #endif
}


void mqtt_client::on_message(const struct mosquitto_message *message)
{
    qDebug()<<"Received a message";
    //int payload_size = MAX_PAYLOAD + 1;
    qDebug()<<MAX_PAYLOAD;            // 50
    qDebug()<<message->payloadlen;    // cca 1400
    qDebug()<<message->topic;
    int payload_size = message->payloadlen+1;


    // v3/flowerpot-arduino@ttn/devices/eui-e0080e1010101010/up

    char buf[payload_size];
    std::string topic = message->topic;

    if(topic.compare("v3/flowerpot-arduino@ttn/devices/eui-e0080e1010101010/up") == 0)//!strcmp(message->topic, PUBLISH_TOPIC))
    {
        memset(buf, 0, payload_size * sizeof(char));

        /* Copy N-1 bytes to ensure always 0 terminated. */
        memcpy(buf, message->payload, message->payloadlen *sizeof(char));//MAX_PAYLOAD * sizeof(char));

        //std::string str_buffer(buf);
        std::string dataPacket = getMyData(std::string(buf));
        qDebug()<<"data packet:"<<QString::fromStdString(dataPacket);
        int h = getHumidity(dataPacket);
        int t = getTemperature(dataPacket);
        int soil = getSoil(dataPacket);
        int light = getLight(dataPacket);
        int cup = getCup(dataPacket);
        int rez = getRez(dataPacket);
        QList<int> data = {t,h,light,soil,cup,rez};
        e->emitData(data);

        //memset(TXBuffer,0,sizeof TXBuffer);//vyprázdnění

        /*Received a message
        50
        1327
        v3/flowerpot-arduino@ttn/devices/eui-e0080e1010101010/up
        data packet: "{\"adc1\":89,\"adc2\":115,\"cup\":3,\"h\":40.8,\"res\":1,\"t\":26.4"
        "40.8"
        "26.4"
        "1\":89"
        "2\":115"
        QList(26, 40, 2, 1)*/

        //TXBuffer[0] = char("d");
        /*
        if(TXBufferSize > 0){
            //snprintf(buf, payload_size, "Turning on...");
            publish(NULL, PUBLISH_TOPIC, TXBufferSize, TXBuffer);
            #ifdef QT_DEBUG
                std::cout << "Data sent" << std::endl;
            #endif
        }
        TXBufferSize = 0;*/

        //printHum(h);

        //":{"adc1":781,"adc2":142,"h":32.8,"t":23.9
        //42


            /*
        // Examples of messages for M2M communications...
        if (!strcmp(buf, "STATUS"))
        {
            snprintf(buf, payload_size, "This is a Status Message...");
            publish(NULL, PUBLISH_TOPIC, strlen(buf), buf);
            #ifdef QT_DEBUG
                std::cout << "Status Request Recieved." << std::endl;
            #endif
        }

        if (!strcmp(buf, "ON"))
        {
            snprintf(buf, payload_size, "Turning on...");
            publish(NULL, PUBLISH_TOPIC, strlen(buf), buf);
            #ifdef QT_DEBUG
                std::cout << "Request to turn on." << std::endl;
            #endif
        }

        if (!strcmp(buf, "OFF"))
        {
            snprintf(buf, payload_size, "Turning off...");
            publish(NULL, PUBLISH_TOPIC, strlen(buf), buf);
            #ifdef QT_DEBUG
                std::cout << "Request to turn off." << std:: endl;
            #endif
        }*/
    }else{
        memset(buf, 0, payload_size * sizeof(char));
        memcpy(buf, message->payload, message->payloadlen *sizeof(char));//MAX_PAYLOAD * sizeof(char));

        qDebug()<<buf;
    }
}
