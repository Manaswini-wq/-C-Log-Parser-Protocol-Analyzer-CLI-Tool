cmake_minimum_required(VERSION 3.14)
project(log_protocol_analyzer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror -pedantic)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(LIB_SOURCES
    src/parser/log_parser.cpp
    src/parser/log_entry.cpp
    src/protocol/packet_decoder.cpp
    src/protocol/can_decoder.cpp
    src/protocol/uart_decoder.cpp
    src/analyzer/stats_engine.cpp
    src/analyzer/filter_engine.cpp
    src/export/csv_exporter.cpp
    src/export/json_exporter.cpp
    src/utils/cli_parser.cpp
    src/utils/timestamp.cpp
)

add_library(analyzer_lib STATIC ${LIB_SOURCES})

add_executable(loganalyzer src/main.cpp)
target_link_libraries(loganalyzer analyzer_lib)

# Tests
enable_testing()
add_executable(test_all
    tests/test_main.cpp
    tests/test_log_parser.cpp
    tests/test_can_decoder.cpp
    tests/test_uart_decoder.cpp
    tests/test_stats_engine.cpp
    tests/test_filter_engine.cpp
)
target_link_libraries(test_all analyzer_lib)
add_test(NAME unit_tests COMMAND test_all)
