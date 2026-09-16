#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#include "parser/log_entry.h"
#include <string>
#include <map>

struct DecodedField {
    std::string name;
    std::string value;
    std::string unit;
};

struct DecodedPacket {
    std::string            protocol_name;
    std::string            message_name;
    std::vector<DecodedField> fields;
};

class PacketDecoder {
public:
    virtual ~PacketDecoder() = default;
    virtual DecodedPacket decode(const LogEntry& entry) = 0;
    virtual bool can_decode(const LogEntry& entry) = 0;
};

#endif
