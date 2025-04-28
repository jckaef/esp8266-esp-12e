#ifndef SENSOR_READING_H
#define SENSOR_READING_H

#include "config.h"


float readDepth() {
  return analogRead(depthSensorPin) * 0.1; // 共用A0引脚
}

float readTemperature() {
  // 模拟温度读数，实际使用需要外部传感器
  return 20.0 + (analogRead(tempSensorPin) * 0.01);
}

void updateSystemStatus(float &cpuUsage, float &voltage, float &current, float &depth, float &temperature) {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= statusUpdateInterval) {
    lastUpdate = millis();
    
    depth = readDepth();
    temperature = readTemperature();
    
    // 模拟电压和电流值
    voltage = 12.0 + random(-5, 5) / 10.0;
    current = 1.0 + random(-2, 2) / 10.0;
    
    // 简化CPU使用率计算
    cpuUsage = random(10, 90);
    
    digitalWrite(statusLed, !digitalRead(statusLed));
  }
}

#endif