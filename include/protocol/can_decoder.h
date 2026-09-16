#ifndef CAN_DECODER_H
#define CAN_DECODER_H

#include "protocol/packet_decoder.h"
#include <map>
#include <functional>

struct CANSignal {
    std::string name;
    uint8_t     start_bit;
    uint8_t     bit_length;
    double      scale;
    double      offset;
    std::string unit;
    bool        is_signed;
};

struct CANMessage {
    uint32_t    id;
    std::string name;
    std::vector<CANSignal> signals;
};

class CANDecoder : public PacketDecoder {
public:
    CANDecoder();

    void add_message(const CANMessage& msg);
    DecodedPacket decode(const LogEntry& entry) override;
    bool can_decode(const LogEntry& entry) override;

    // Extract raw signal value from CAN data bytes
    static int64_t extract_signal(const std::vector<uint8_t>& data,
                                  uint8_t start_bit, uint8_t bit_length,
                                  bool is_signed);

private:
    std::map<uint32_t, CANMessage> message_db_;
    void load_default_messages();
};

#endif
