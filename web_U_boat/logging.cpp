#include "logging.h"

LogBuffer systemLogs;  // 实际定义在这里

void LogBuffer::add(const char* msg, LogType type) {
    if(count < MAX_LOG_ENTRIES) {
        strncpy(entries[head].message, msg, LOG_ENTRY_SIZE-1);
        entries[head].message[LOG_ENTRY_SIZE-1] = '\0';
        entries[head].type = type;
        entries[head].timestamp = millis();
        head = (head + 1) % MAX_LOG_ENTRIES;
        count++;
    }
}

const LogEntry* LogBuffer::getEntry(uint8_t index) const {
    if(index >= count) return nullptr;
    return &entries[(head - count + index) % MAX_LOG_ENTRIES];
}

uint8_t LogBuffer::size() const { 
    return count; 
}

void addLog(const char* source, const char* message, LogType type) {
    char logMsg[LOG_ENTRY_SIZE];
    snprintf(logMsg, LOG_ENTRY_SIZE, "[%s] %s", source, message);
    systemLogs.add(logMsg, type);
}