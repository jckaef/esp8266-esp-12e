#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "config.h"

// 停止所有电机
void allMotorsStop() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  digitalWrite(grabberPin, LOW);
}

// 紧急停止
void emergencyStop() {
  allMotorsStop();
  digitalWrite(emergencyStopPin, LOW);
  delay(1000);
  digitalWrite(emergencyStopPin, HIGH);
}

// 执行命令
void executeCommand(String command) {
  if (command == "forward") {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  } else if (command == "backward") {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  } else if (command == "left") {
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  } else if (command == "right") {
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  } else if (command == "dive") {
    digitalWrite(divePin, HIGH);
    delay(100);
    digitalWrite(divePin, LOW);
  } else if (command == "rise") {
    digitalWrite(risePin, HIGH);
    delay(100);
    digitalWrite(risePin, LOW);
  } else if (command == "stop") {
    allMotorsStop();
  } else if (command == "grab") {
    digitalWrite(grabberPin, HIGH);
  } else if (command == "release") {
    digitalWrite(grabberPin, LOW);
  } else if (command == "hover") {
    digitalWrite(hoverPin, HIGH);
    delay(100);
    digitalWrite(hoverPin, LOW);
  } else if (command == "lock") {
    digitalWrite(lockPin, HIGH);
    delay(100);
    digitalWrite(lockPin, LOW);
  } else if (command == "fire") {
    digitalWrite(firePin, HIGH);
    delay(100);
    digitalWrite(firePin, LOW);
  }
}

// 检查是否为自动模式命令
bool isAutoCommand(String command) {
  return command == "hover" || command == "lock" || command == "fire" || command == "self_check";
}

#endif