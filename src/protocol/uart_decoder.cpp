#include "protocol/uart_decoder.h"
#include <sstream>
#include <iomanip>

bool UARTDecoder::can_decode(const LogEntry& entry) {
    return entry.protocol == ProtocolType::UART && !entry.uart_data.empty();
}

DecodedPacket UARTDecoder::decode(const LogEntry& entry) {
    DecodedPacket pkt;
    pkt.protocol_name = "UART";
    pkt.message_name = detect_frame_type(entry.uart_data);

    pkt.fields.push_back({"Port", std::to_string(entry.uart_port), ""});
    pkt.fields.push_back({"Direction", entry.uart_direction, ""});
    pkt.fields.push_back({"Length", std::to_string(entry.uart_data.size()), "bytes"});
    pkt.fields.push_back({"ASCII", decode_ascii(entry.uart_data), ""});

    // Hex dump
    std::ostringstream hex;
    for (auto b : entry.uart_data) {
        hex << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    pkt.fields.push_back({"Hex", hex.str(), ""});

    return pkt;
}

std::string UARTDecoder::decode_ascii(const std::vector<uint8_t>& data) {
    std::string result;
    for (auto b : data) {
        result += (b >= 0x20 && b <= 0x7E) ? static_cast<char>(b) : '.';
    }
    return result;
}

std::string UARTDecoder::detect_frame_type(const std::vector<uint8_t>& data) {
    if (data.empty()) return "Empty";
    if (data[0] == 0xA5) return "A5_Command";
    if (data[0] == 0x01 && data.size() >= 4) return "Modbus_Request";
    if (data.size() >= 2 && data[0] == 0x7E) return "HDLC_Frame";
    // Check if it's printable ASCII
    bool all_ascii = true;
    for (auto b : data) {
        if (b < 0x20 || b > 0x7E) { all_ascii = false; break; }
    }
    if (all_ascii) return "ASCII_Text";
    return "Binary_Data";
}
