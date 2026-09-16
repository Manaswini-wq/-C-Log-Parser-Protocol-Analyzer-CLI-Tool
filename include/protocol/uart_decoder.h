#ifndef UART_DECODER_H
#define UART_DECODER_H

#include "protocol/packet_decoder.h"

class UARTDecoder : public PacketDecoder {
public:
    DecodedPacket decode(const LogEntry& entry) override;
    bool can_decode(const LogEntry& entry) override;

private:
    std::string decode_ascii(const std::vector<uint8_t>& data);
    std::string detect_frame_type(const std::vector<uint8_t>& data);
};

#endif
