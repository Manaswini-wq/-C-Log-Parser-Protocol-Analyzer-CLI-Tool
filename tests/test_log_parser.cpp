#include "test_framework.h"
#include "parser/log_parser.h"

TEST_SUITE(test_log_parser) {
    LogParser parser;

    // Test CAN line parsing
    auto e1 = parser.parse_line("0.001234 CAN 0x1A0 [8] 01 02 03 04 05 06 07 08");
    ASSERT_EQ(e1.protocol, ProtocolType::CAN);
    ASSERT_EQ(e1.can_id, 0x1A0u);
    ASSERT_EQ(e1.can_dlc, 8);
    ASSERT_EQ(e1.can_data.size(), 8u);
    ASSERT_EQ(e1.can_data[0], 0x01);
    ASSERT_EQ(e1.can_data[7], 0x08);
    ASSERT_FLOAT_EQ(e1.timestamp_sec, 0.001234, 0.0001);

    // Test UART line parsing
    auto e2 = parser.parse_line("1.500000 UART0 TX 48 65 6C 6C 6F");
    ASSERT_EQ(e2.protocol, ProtocolType::UART);
    ASSERT_EQ(e2.uart_port, 0);
    ASSERT_EQ(e2.uart_direction, "TX");
    ASSERT_EQ(e2.uart_data.size(), 5u);
    ASSERT_EQ(e2.uart_data[0], 0x48);  // 'H'

    // Test extended CAN ID
    auto e3 = parser.parse_line("2.000000 CAN 0x18FEF100 [8] AA BB CC DD EE FF 00 11");
    ASSERT_TRUE(e3.can_extended);
    ASSERT_EQ(e3.can_id, 0x18FEF100u);
}
