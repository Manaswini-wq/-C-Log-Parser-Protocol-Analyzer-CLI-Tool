#include "test_framework.h"
#include "protocol/uart_decoder.h"

TEST_SUITE(test_uart_decoder) {
    UARTDecoder decoder;

    // Test ASCII text detection
    LogEntry ascii_entry;
    ascii_entry.protocol = ProtocolType::UART;
    ascii_entry.uart_port = 0;
    ascii_entry.uart_direction = "TX";
    ascii_entry.uart_data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"

    ASSERT_TRUE(decoder.can_decode(ascii_entry));
    auto pkt = decoder.decode(ascii_entry);
    ASSERT_EQ(pkt.message_name, "ASCII_Text");

    // Test A5 command detection
    LogEntry a5_entry;
    a5_entry.protocol = ProtocolType::UART;
    a5_entry.uart_data = {0xA5, 0x01, 0x02, 0x03};
    auto a5_pkt = decoder.decode(a5_entry);
    ASSERT_EQ(a5_pkt.message_name, "A5_Command");

    // Test Modbus detection
    LogEntry modbus_entry;
    modbus_entry.protocol = ProtocolType::UART;
    modbus_entry.uart_data = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    auto mb_pkt = decoder.decode(modbus_entry);
    ASSERT_EQ(mb_pkt.message_name, "Modbus_Request");

    // Test empty data
    LogEntry empty;
    empty.protocol = ProtocolType::UART;
    ASSERT_TRUE(!decoder.can_decode(empty));
}
