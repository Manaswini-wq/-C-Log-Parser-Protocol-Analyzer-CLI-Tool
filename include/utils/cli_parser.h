#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <map>
#include <vector>

struct CLIOptions {
    std::string input_file;
    std::string output_file;
    std::string format;          // "csv" or "json"
    std::string filter_protocol; // "can", "uart", or ""
    uint32_t    filter_can_id;   // 0 = no filter
    double      time_start;      // 0 = no filter
    double      time_end;        // 0 = no filter
    bool        show_stats;
    bool        decode;
    bool        verbose;
    bool        help;
};

CLIOptions parse_cli(int argc, char* argv[]);
void       print_usage(const char* program_name);

#endif
