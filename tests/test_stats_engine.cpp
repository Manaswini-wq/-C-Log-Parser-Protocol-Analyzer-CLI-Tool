#include "test_framework.h"
#include "analyzer/stats_engine.h"

TEST_SUITE(test_stats_engine) {
    std::vector<LogEntry> entries;

    for (int i = 0; i < 100; i++) {
        LogEntry e;
        e.timestamp_sec = i * 0.01;
        e.protocol = (i % 3 == 0) ? ProtocolType::UART : ProtocolType::CAN;
        e.can_id = (i % 2 == 0) ? 0x1A0 : 0x3B0;
        entries.push_back(e);
    }

    StatsEngine engine;
    engine.analyze(entries);

    auto stats = engine.get_overall_stats();
    ASSERT_EQ(stats.total_packets, 100u);
    ASSERT_FLOAT_EQ(stats.first_timestamp, 0.0, 0.001);
    ASSERT_GT(stats.packets_per_second, 0.0);

    auto proto = engine.get_protocol_counts();
    ASSERT_GT(proto[ProtocolType::CAN], 0u);
    ASSERT_GT(proto[ProtocolType::UART], 0u);

    auto can = engine.get_can_stats();
    ASSERT_TRUE(can.find(0x1A0) != can.end());
    ASSERT_TRUE(can.find(0x3B0) != can.end());
    ASSERT_GT(can[0x1A0].count, 0u);

    // Test throughput timeline
    auto timeline = engine.get_throughput_timeline(0.1);
    ASSERT_GT(timeline.size(), 0u);
}
