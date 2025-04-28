#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "config.h"
#include "motor_control.h"
#include "sensor_reading.h"
#include "logging.h"
#include "web_interface.h"
#include "eeprom_utils.h"

ESP8266WebServer server(serverPort);

// 系统状态变量
bool systemStatus = true;
bool autoMode = false;
bool wifiConnected = false;
String wifiSsid = "";
float cpuUsage = 0.0;
float voltage = 12.0;
float current = 1.5;
float depth = 0.0;
float temperature = 25.0;

// 移除这行：LogBuffer systemLogs;

void setup() {
  Serial.begin(115200);
  initEEPROM();
  
  // 初始化所有引脚
  pinMode(statusLed, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(grabberPin, OUTPUT);
  pinMode(emergencyStopPin, OUTPUT);
  pinMode(divePin, OUTPUT);
  pinMode(risePin, OUTPUT);
  pinMode(hoverPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  pinMode(firePin, OUTPUT);
  
  allMotorsStop();
  
  connectWiFi();
  setupServer();
  
  server.begin();
  Serial.println("服务器已启动");
}

void loop() {
  server.handleClient();
  // 直接使用sensor_reading.h中定义的updateSystemStatus
  updateSystemStatus(cpuUsage, voltage, current, depth, temperature);
}

void connectWiFi() {
  WiFiManager wifiManager;
  
  // 设置自定义参数
  WiFiManagerParameter custom_mqtt_server("server", "MQTT服务器地址", mqtt_server, 40);
  wifiManager.addParameter(&custom_mqtt_server);

  // 设置超时时间(秒)
  wifiManager.setConfigPortalTimeout(180);
  
  // 尝试连接已保存的WiFi，如果失败则启动配置门户
  if(!wifiManager.autoConnect("U-Boat_AP")) {
    Serial.println("连接超时，重置ESP");
    ESP.reset();
    delay(1000);
  }
  
  // 获取自定义参数值
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  
  // 记录连接状态
  wifiSsid = WiFi.SSID();
  wifiConnected = true;
  // 修改日志调用方式，将String转换为const char*
  String logMsg = "连接成功: " + wifiSsid;
  addLog("WiFi", logMsg.c_str(), LOG_INFO);
}

void setupServer() {
  // 提供HTML页面
  server.on("/", HTTP_GET, []() {
    String html = FPSTR(web_U_boat_html);
    server.send(200, "text/html", html);
  });

  // API端点
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/test", HTTP_POST, handleCommand);
  server.on("/api/emergency_stop", HTTP_POST, handleEmergencyStop);
  server.on("/api/mode", HTTP_POST, handleModeChange);
}

void handleStatus() {
  DynamicJsonDocument doc(256);
  doc["system_status"] = systemStatus ? "正常" : "故障";
  doc["cpu_usage"] = cpuUsage;
  doc["voltage"] = voltage;
  doc["current"] = current;
  doc["depth"] = depth;
  doc["temperature"] = temperature;
  doc["wifi_connected"] = wifiConnected;
  doc["wifi_ssid"] = wifiSsid;
  doc["auto_mode"] = autoMode;

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleCommand() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    String command = doc["command"].as<String>();

    // 修改模式检查逻辑
    if (autoMode) {
      if (command == "hover" || command == "lock" || command == "fire" || command == "self_check") {
        executeCommand(command);
        server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"命令执行成功\"}");
        return;
      }
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"当前为自动模式，不能执行手动命令\"}");
    } else {
      if (command == "forward" || command == "backward" || command == "left" || command == "right" || 
          command == "dive" || command == "rise" || command == "stop" || command == "grab" || command == "release") {
        executeCommand(command);
        server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"命令执行成功\"}");
        return;
      }
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"当前为手动模式，不能执行自动命令\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"无效命令\"}");
  }
}

void handleEmergencyStop() {
  emergencyStop();
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"紧急停止已激活\"}");
}

void handleModeChange() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    autoMode = doc["auto_mode"];
    
    allMotorsStop();
    
    // 添加状态更新
    String response = "{\"status\":\"success\",\"message\":\"模式已切换为";
    response += autoMode ? "自动" : "手动";
    response += "\",\"auto_mode\":";
    response += autoMode ? "true" : "false";
    response += "}";
    
    server.send(200, "application/json", response);
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"无效请求\"}");
  }
}

// 完全移除以下重复定义的updateSystemStatus函数
/*
void updateSystemStatus(float &cpuUsage, float &voltage, float &current, float &depth, float &temperature) {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= statusUpdateInterval) {
    lastUpdate = millis();
    
    // 使用sensor_reading.h中的函数
    depth = readDepth();
    temperature = readTemperature();
    
    // 模拟数据
    cpuUsage = random(10, 90);
    voltage = 10.0 + random(0, 40) / 10.0;
    current = 1.0 + random(0, 20) / 10.0;
    
    digitalWrite(statusLed, !digitalRead(statusLed));
  }
}
*/

