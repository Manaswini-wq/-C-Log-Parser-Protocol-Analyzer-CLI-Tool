#include "utils/cli_parser.h"
#include "utils/timestamp.h"
#include "parser/log_parser.h"
#include "analyzer/stats_engine.h"
#include "analyzer/filter_engine.h"
#include "protocol/can_decoder.h"
#include "protocol/uart_decoder.h"
#include "export/csv_exporter.h"
#include "export/json_exporter.h"

#include <iostream>
#include <iomanip>
#include <memory>

static void print_stats(const StatsEngine& engine) {
    auto stats = engine.get_overall_stats();
    std::cout << "\n=== Log Statistics ===\n"
              << "Total packets:  " << stats.total_packets << "\n"
              << "Duration:       " << format_duration(stats.last_timestamp - stats.first_timestamp) << "\n"
              << "Throughput:     " << std::fixed << std::setprecision(1)
              << stats.packets_per_second << " pkt/s\n";

    auto proto = engine.get_protocol_counts();
    std::cout << "\nBy Protocol:\n";
    for (auto& [type, count] : proto) {
        std::cout << "  " << (type == ProtocolType::CAN ? "CAN" : "UART")
                  << ": " << count << "\n";
    }

    auto can = engine.get_can_stats();
    if (!can.empty()) {
        std::cout << "\nCAN ID Summary:\n"
                  << std::left << std::setw(10) << "ID"
                  << std::setw(8) << "Count"
                  << std::setw(12) << "Avg(ms)"
                  << std::setw(12) << "Min(ms)"
                  << "Max(ms)\n";
        for (auto& [id, cs] : can) {
            std::cout << "0x" << std::hex << std::setw(8) << std::left << id << std::dec
                      << std::setw(8) << cs.count
                      << std::fixed << std::setprecision(2)
                      << std::setw(12) << cs.avg_interval_ms
                      << std::setw(12) << cs.min_interval_ms
                      << cs.max_interval_ms << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    CLIOptions opts = parse_cli(argc, argv);

    if (opts.help || opts.input_file.empty()) {
        print_usage(argv[0]);
        return opts.help ? 0 : 1;
    }

    // Parse
    LogParser parser;
    std::vector<LogEntry> entries;
    try {
        entries = parser.parse_file(opts.input_file);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (opts.verbose)
        std::cout << "Parsed " << entries.size() << " entries\n";

    // Filter
    FilterEngine filter;
    if (opts.filter_protocol == "can") filter.protocol(ProtocolType::CAN);
    else if (opts.filter_protocol == "uart") filter.protocol(ProtocolType::UART);
    if (opts.filter_can_id != 0) filter.can_id(opts.filter_can_id);
    if (opts.time_start > 0 || opts.time_end > 0)
        filter.time_range(opts.time_start, opts.time_end > 0 ? opts.time_end : 1e9);

    auto filtered = filter.apply(entries);
    if (opts.verbose)
        std::cout << "After filtering: " << filtered.size() << " entries\n";

    // Stats
    if (opts.show_stats) {
        StatsEngine stats;
        stats.analyze(filtered);
        print_stats(stats);
    }

    // Decode
    if (opts.decode) {
        CANDecoder can_dec;
        UARTDecoder uart_dec;
        std::vector<PacketDecoder*> decoders = {&can_dec, &uart_dec};

        std::vector<DecodedPacket> decoded;
        for (const auto& e : filtered) {
            for (auto* dec : decoders) {
                if (dec->can_decode(e)) {
                    decoded.push_back(dec->decode(e));
                    break;
                }
            }
        }

        if (!opts.output_file.empty()) {
            CSVExporter::export_decoded(opts.output_file, decoded);
        } else {
            for (const auto& pkt : decoded) {
                std::cout << "[" << pkt.protocol_name << "] " << pkt.message_name << ": ";
                for (const auto& f : pkt.fields) {
                    std::cout << f.name << "=" << f.value << f.unit << " ";
                }
                std::cout << "\n";
            }
        }
    }

    // Export raw
    if (!opts.output_file.empty() && !opts.decode) {
        if (opts.format == "json")
            JSONExporter::export_entries(opts.output_file, filtered);
        else
            CSVExporter::export_entries(opts.output_file, filtered);
    }

    return 0;
}
