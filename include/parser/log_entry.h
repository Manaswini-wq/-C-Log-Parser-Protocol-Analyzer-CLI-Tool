#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>
#include <vector>
#include <cstdint>

enum class ProtocolType {
    CAN,
    UART,
    UNKNOWN
};

struct LogEntry {
    double       timestamp_sec;
    ProtocolType protocol;
    std::string  raw_line;

    // CAN fields
    uint32_t     can_id;
    bool         can_extended;
    uint8_t      can_dlc;
    std::vector<uint8_t> can_data;

    // UART fields
    uint8_t      uart_port;
    std::string  uart_direction;  // "TX" or "RX"
    std::vector<uint8_t> uart_data;

    LogEntry() : timestamp_sec(0), protocol(ProtocolType::UNKNOWN),
                 can_id(0), can_extended(false), can_dlc(0), uart_port(0) {}
};

#endif
