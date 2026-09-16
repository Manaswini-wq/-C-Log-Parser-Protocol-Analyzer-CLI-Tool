#include "utils/cli_parser.h"
#include <iostream>
#include <cstring>

CLIOptions parse_cli(int argc, char* argv[]) {
    CLIOptions opts = {};
    opts.format = "csv";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") { opts.help = true; }
        else if (arg == "-v" || arg == "--verbose") { opts.verbose = true; }
        else if (arg == "-s" || arg == "--stats") { opts.show_stats = true; }
        else if (arg == "-d" || arg == "--decode") { opts.decode = true; }
        else if ((arg == "-i" || arg == "--input") && i + 1 < argc) { opts.input_file = argv[++i]; }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) { opts.output_file = argv[++i]; }
        else if ((arg == "-f" || arg == "--format") && i + 1 < argc) { opts.format = argv[++i]; }
        else if ((arg == "-p" || arg == "--protocol") && i + 1 < argc) { opts.filter_protocol = argv[++i]; }
        else if (arg == "--can-id" && i + 1 < argc) {
            opts.filter_can_id = static_cast<uint32_t>(std::stoul(argv[++i], nullptr, 0));
        }
        else if (arg == "--time-start" && i + 1 < argc) { opts.time_start = std::stod(argv[++i]); }
        else if (arg == "--time-end" && i + 1 < argc) { opts.time_end = std::stod(argv[++i]); }
        else if (opts.input_file.empty()) { opts.input_file = arg; }
    }
    return opts;
}

void print_usage(const char* name) {
    std::cout << "Usage: " << name << " [OPTIONS] <input_file>\n\n"
              << "Options:\n"
              << "  -i, --input FILE      Input log file\n"
              << "  -o, --output FILE     Output file (default: stdout)\n"
              << "  -f, --format FMT      Output format: csv, json (default: csv)\n"
              << "  -p, --protocol PROTO  Filter: can, uart\n"
              << "  --can-id ID           Filter by CAN ID (hex: 0x1A0)\n"
              << "  --time-start SEC      Filter start time\n"
              << "  --time-end SEC        Filter end time\n"
              << "  -d, --decode          Decode protocol fields\n"
              << "  -s, --stats           Show statistics summary\n"
              << "  -v, --verbose         Verbose output\n"
              << "  -h, --help            Show this help\n";
}
