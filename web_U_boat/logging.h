#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

#define MAX_LOG_ENTRIES 50
#define LOG_ENTRY_SIZE 128

enum LogType {
    LOG_INFO = 'I',
    LOG_WARNING = 'W', 
    LOG_ERROR = 'E',
    LOG_DEBUG = 'D'
};

struct LogEntry {
    char message[LOG_ENTRY_SIZE];
    LogType type;
    unsigned long timestamp;
};

class LogBuffer {
private:
    LogEntry entries[MAX_LOG_ENTRIES];
    uint8_t count = 0;
    uint8_t head = 0;
    
public:
    void add(const char* msg, LogType type);
    const LogEntry* getEntry(uint8_t index) const;
    uint8_t size() const;
};

extern LogBuffer systemLogs;  // 声明为外部变量

#endif

void addLog(const char* source, const char* message, LogType type);