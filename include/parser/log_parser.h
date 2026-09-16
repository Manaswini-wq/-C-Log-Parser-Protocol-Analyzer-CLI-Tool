#ifndef LOG_PARSER_H
#define LOG_PARSER_H

#include "parser/log_entry.h"
#include <string>
#include <vector>

class LogParser {
public:
    std::vector<LogEntry> parse_file(const std::string& filepath);
    LogEntry parse_line(const std::string& line);

private:
    LogEntry parse_can_line(const std::string& line);
    LogEntry parse_uart_line(const std::string& line);
    ProtocolType detect_protocol(const std::string& line);
    double parse_timestamp(const std::string& token);
    std::vector<uint8_t> parse_hex_bytes(const std::string& hex_str);
};

#endif
