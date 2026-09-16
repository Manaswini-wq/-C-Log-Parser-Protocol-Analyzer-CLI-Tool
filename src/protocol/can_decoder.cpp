#include "protocol/can_decoder.h"
#include <sstream>
#include <cmath>

CANDecoder::CANDecoder() {
    load_default_messages();
}

void CANDecoder::add_message(const CANMessage& msg) {
    message_db_[msg.id] = msg;
}

bool CANDecoder::can_decode(const LogEntry& entry) {
    return entry.protocol == ProtocolType::CAN &&
           message_db_.find(entry.can_id) != message_db_.end();
}

int64_t CANDecoder::extract_signal(const std::vector<uint8_t>& data,
                                   uint8_t start_bit, uint8_t bit_length,
                                   bool is_signed) {
    uint64_t raw = 0;
    for (uint8_t i = 0; i < bit_length; i++) {
        uint8_t bit_pos = start_bit + i;
        uint8_t byte_idx = bit_pos / 8;
        uint8_t bit_idx = bit_pos % 8;
        if (byte_idx < data.size()) {
            if (data[byte_idx] & (1 << bit_idx)) {
                raw |= (1ULL << i);
            }
        }
    }

    if (is_signed && (raw & (1ULL << (bit_length - 1)))) {
        // Sign extend
        raw |= ~((1ULL << bit_length) - 1);
        return static_cast<int64_t>(raw);
    }
    return static_cast<int64_t>(raw);
}

DecodedPacket CANDecoder::decode(const LogEntry& entry) {
    DecodedPacket pkt;
    pkt.protocol_name = "CAN";

    auto it = message_db_.find(entry.can_id);
    if (it == message_db_.end()) {
        pkt.message_name = "Unknown";
        return pkt;
    }

    const CANMessage& msg = it->second;
    pkt.message_name = msg.name;

    for (const auto& sig : msg.signals) {
        int64_t raw_val = extract_signal(entry.can_data, sig.start_bit,
                                         sig.bit_length, sig.is_signed);
        double phys_val = raw_val * sig.scale + sig.offset;

        std::ostringstream oss;
        oss << phys_val;

        pkt.fields.push_back({sig.name, oss.str(), sig.unit});
    }
    return pkt;
}

void CANDecoder::load_default_messages() {
    // Example: Engine RPM and coolant temp (OBD-II style)
    CANMessage engine_msg;
    engine_msg.id = 0x1A0;
    engine_msg.name = "EngineData";
    engine_msg.signals = {
        {"RPM",          0, 16, 0.25, 0.0, "rpm", false},
        {"CoolantTemp",  16, 8, 1.0, -40.0, "degC", false},
        {"ThrottlePos",  24, 8, 0.392157, 0.0, "%", false},
    };
    add_message(engine_msg);

    CANMessage battery_msg;
    battery_msg.id = 0x3B0;
    battery_msg.name = "BatteryStatus";
    battery_msg.signals = {
        {"Voltage",  0, 16, 0.01, 0.0, "V", false},
        {"Current",  16, 16, 0.1, -3200.0, "A", true},
        {"SOC",      32, 8, 0.5, 0.0, "%", false},
    };
    add_message(battery_msg);

    CANMessage sensor_msg;
    sensor_msg.id = 0x200;
    sensor_msg.name = "SensorHub";
    sensor_msg.signals = {
        {"Temperature", 0, 16, 0.01, -40.0, "degC", true},
        {"Humidity",    16, 16, 0.01, 0.0, "%", false},
        {"Pressure",    32, 16, 1.0, 0.0, "hPa", false},
    };
    add_message(sensor_msg);
}
