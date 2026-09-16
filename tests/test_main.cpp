#include "test_framework.h"

extern void test_log_parser();
extern void test_can_decoder();
extern void test_uart_decoder();
extern void test_stats_engine();
extern void test_filter_engine();

int main() {
    std::cout << "\n===== Log Protocol Analyzer — Unit Tests =====\n\n";

    RUN_SUITE(test_log_parser);
    RUN_SUITE(test_can_decoder);
    RUN_SUITE(test_uart_decoder);
    RUN_SUITE(test_stats_engine);
    RUN_SUITE(test_filter_engine);

    TEST_REPORT();
    return (g_tests_failed > 0) ? 1 : 0;
}
