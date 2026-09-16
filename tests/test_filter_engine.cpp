#include "test_framework.h"
#include "analyzer/filter_engine.h"

TEST_SUITE(test_filter_engine) {
    std::vector<LogEntry> entries;

    // Create mixed entries
    for (int i = 0; i < 20; i++) {
        LogEntry e;
        e.timestamp_sec = i * 0.5;
        e.protocol = (i < 10) ? ProtocolType::CAN : ProtocolType::UART;
        e.can_id = (i % 2 == 0) ? 0x1A0 : 0x3B0;
        e.can_data = {static_cast<uint8_t>(i), 0x00};
        entries.push_back(e);
    }

    // Filter by protocol
    FilterEngine f1;
    f1.protocol(ProtocolType::CAN);
    auto result = f1.apply(entries);
    ASSERT_EQ(result.size(), 10u);

    // Filter by CAN ID
    FilterEngine f2;
    f2.can_id(0x1A0);
    result = f2.apply(entries);
    for (const auto& e : result) {
        ASSERT_EQ(e.can_id, 0x1A0u);
    }

    // Filter by time range
    FilterEngine f3;
    f3.time_range(2.0, 4.0);
    result = f3.apply(entries);
    for (const auto& e : result) {
        ASSERT_TRUE(e.timestamp_sec >= 2.0 && e.timestamp_sec <= 4.0);
    }

    // Chained filters — protocol AND can_id
    FilterEngine f4;
    f4.protocol(ProtocolType::CAN).can_id(0x1A0);
    result = f4.apply(entries);
    for (const auto& e : result) {
        ASSERT_EQ(e.protocol, ProtocolType::CAN);
        ASSERT_EQ(e.can_id, 0x1A0u);
    }

    // Data byte filter
    FilterEngine f5;
    f5.data_byte(0, 0x04);
    result = f5.apply(entries);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0].can_data[0], 0x04);

    // Clear and reuse
    f5.clear();
    result = f5.apply(entries);
    ASSERT_EQ(result.size(), 20u);
}
