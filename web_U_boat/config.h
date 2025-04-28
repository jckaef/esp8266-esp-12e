#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi配置
const char* ssid = "U-Boat_AP";
const char* password = "12345678";

// 服务器配置
const int serverPort = 80;

// 引脚配置
const int motorPin1 = D1;
const int motorPin2 = D2;
const int motorPin3 = D3;
const int motorPin4 = D4;
const int grabberPin = D5;
const int emergencyStopPin = D6;
const int divePin = D7;
const int risePin = D8;
const int hoverPin = D0;
const int lockPin = D9;
const int firePin = D10;

// 传感器引脚
const int depthSensorPin = A0;
const int tempSensorPin = A0; 

// 状态LED
const int statusLed = LED_BUILTIN;

// 系统参数
const unsigned long statusUpdateInterval = 1000;
const int maxLogEntries = 50;

// EEPROM相关函数声明
void initEEPROM();
String readWiFiSSID();
String readWiFiPassword();
void writeWiFiCreds(String ssid, String password);

// MQTT配置
char mqtt_server[40] = "mqtt.example.com";  // 默认MQTT服务器地址

#endif