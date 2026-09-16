#include "parser/log_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

std::vector<LogEntry> LogParser::parse_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::vector<LogEntry> entries;
    std::string line;
    size_t line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        if (line.empty() || line[0] == '#') continue;  // skip comments/blanks

        try {
            LogEntry entry = parse_line(line);
            entry.raw_line = line;
            entries.push_back(entry);
        } catch (...) {
            // Skip unparseable lines
        }
    }
    return entries;
}

LogEntry LogParser::parse_line(const std::string& line) {
    ProtocolType proto = detect_protocol(line);
    switch (proto) {
        case ProtocolType::CAN:  return parse_can_line(line);
        case ProtocolType::UART: return parse_uart_line(line);
        default:
            throw std::runtime_error("Unknown protocol");
    }
}

ProtocolType LogParser::detect_protocol(const std::string& line) {
    // CAN format: "0.001234 CAN 0x1A0 [8] 01 02 03 04 05 06 07 08"
    if (line.find("CAN") != std::string::npos) return ProtocolType::CAN;
    // UART format: "0.001234 UART0 TX 48 65 6C 6C 6F"
    if (line.find("UART") != std::string::npos) return ProtocolType::UART;
    return ProtocolType::UNKNOWN;
}

double LogParser::parse_timestamp(const std::string& token) {
    return std::stod(token);
}

std::vector<uint8_t> LogParser::parse_hex_bytes(const std::string& hex_str) {
    std::vector<uint8_t> bytes;
    std::istringstream iss(hex_str);
    std::string byte_str;
    while (iss >> byte_str) {
        bytes.push_back(static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16)));
    }
    return bytes;
}

LogEntry LogParser::parse_can_line(const std::string& line) {
    // Format: "0.001234 CAN 0x1A0 [8] 01 02 03 04 05 06 07 08"
    LogEntry entry;
    entry.protocol = ProtocolType::CAN;

    std::istringstream iss(line);
    std::string ts_str, proto_str, id_str, dlc_str;

    iss >> ts_str >> proto_str >> id_str >> dlc_str;
    entry.timestamp_sec = parse_timestamp(ts_str);

    // Parse CAN ID — handle "0x1A0" or "1A0"
    if (id_str.substr(0, 2) == "0x" || id_str.substr(0, 2) == "0X")
        id_str = id_str.substr(2);
    entry.can_id = static_cast<uint32_t>(std::stoul(id_str, nullptr, 16));
    entry.can_extended = (entry.can_id > 0x7FF);

    // Parse DLC — "[8]"
    if (dlc_str.front() == '[' && dlc_str.back() == ']') {
        dlc_str = dlc_str.substr(1, dlc_str.size() - 2);
    }
    entry.can_dlc = static_cast<uint8_t>(std::stoul(dlc_str));

    // Parse data bytes
    std::string remaining;
    std::getline(iss, remaining);
    entry.can_data = parse_hex_bytes(remaining);

    return entry;
}

LogEntry LogParser::parse_uart_line(const std::string& line) {
    // Format: "0.001234 UART0 TX 48 65 6C 6C 6F"
    LogEntry entry;
    entry.protocol = ProtocolType::UART;

    std::istringstream iss(line);
    std::string ts_str, port_str, dir_str;

    iss >> ts_str >> port_str >> dir_str;
    entry.timestamp_sec = parse_timestamp(ts_str);

    // Parse port number from "UART0", "UART1", etc.
    if (port_str.size() > 4) {
        entry.uart_port = static_cast<uint8_t>(port_str[4] - '0');
    }
    entry.uart_direction = dir_str;

    std::string remaining;
    std::getline(iss, remaining);
    entry.uart_data = parse_hex_bytes(remaining);

    return entry;
}
