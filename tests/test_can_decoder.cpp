#include "test_framework.h"
#include "protocol/can_decoder.h"

TEST_SUITE(test_can_decoder) {
    CANDecoder decoder;

    // Test signal extraction — unsigned
    std::vector<uint8_t> data = {0xE8, 0x03, 0x50, 0x80, 0, 0, 0, 0};
    // Bytes [0:1] = 0x03E8 = 1000 in little-endian
    int64_t rpm_raw = CANDecoder::extract_signal(data, 0, 16, false);
    ASSERT_EQ(rpm_raw, 1000);

    // Test signal extraction — signed negative
    std::vector<uint8_t> signed_data = {0x00, 0x80, 0, 0, 0, 0, 0, 0};
    // bit pattern for -32768 in 16-bit signed
    int64_t signed_val = CANDecoder::extract_signal(signed_data, 0, 16, true);
    ASSERT_EQ(signed_val, -32768);

    // Test decode known message (0x1A0 = EngineData)
    LogEntry entry;
    entry.protocol = ProtocolType::CAN;
    entry.can_id = 0x1A0;
    entry.can_data = {0xE8, 0x03, 0x50, 0x80, 0, 0, 0, 0};

    ASSERT_TRUE(decoder.can_decode(entry));
    auto pkt = decoder.decode(entry);
    ASSERT_EQ(pkt.message_name, "EngineData");
    ASSERT_GT(pkt.fields.size(), 0u);

    // Test unknown message
    LogEntry unknown;
    unknown.protocol = ProtocolType::CAN;
    unknown.can_id = 0xFFF;
    unknown.can_data = {0x01};
    ASSERT_TRUE(!decoder.can_decode(unknown));
}
