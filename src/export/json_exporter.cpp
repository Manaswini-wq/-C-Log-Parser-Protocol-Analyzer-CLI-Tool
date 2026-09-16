#include "export/json_exporter.h"
#include <fstream>
#include <iomanip>
#include <sstream>

static std::string escape_json(const std::string& s) {
    std::string result;
    for (char c : s) {
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else result += c;
    }
    return result;
}

bool JSONExporter::export_entries(const std::string& filepath,
                                   const std::vector<LogEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "[\n";
    for (size_t i = 0; i < entries.size(); i++) {
        const auto& e = entries[i];
        file << "  {\"timestamp\": " << std::fixed << std::setprecision(6)
             << e.timestamp_sec
             << ", \"protocol\": \"" << (e.protocol == ProtocolType::CAN ? "CAN" : "UART")
             << "\"";

        if (e.protocol == ProtocolType::CAN) {
            file << ", \"can_id\": " << e.can_id
                 << ", \"dlc\": " << (int)e.can_dlc << ", \"data\": [";
            for (size_t j = 0; j < e.can_data.size(); j++) {
                if (j > 0) file << ", ";
                file << (int)e.can_data[j];
            }
            file << "]";
        } else {
            file << ", \"port\": " << (int)e.uart_port
                 << ", \"direction\": \"" << e.uart_direction << "\"";
        }
        file << "}" << (i + 1 < entries.size() ? "," : "") << "\n";
    }
    file << "]\n";
    return true;
}

bool JSONExporter::export_stats(const std::string& filepath,
                                 const StatsEngine& engine) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    auto stats = engine.get_overall_stats();
    file << "{\n  \"total_packets\": " << stats.total_packets
         << ",\n  \"duration_sec\": " << std::fixed << std::setprecision(3)
         << (stats.last_timestamp - stats.first_timestamp)
         << ",\n  \"packets_per_second\": " << std::setprecision(1)
         << stats.packets_per_second
         << ",\n  \"can_ids\": {\n";

    auto can_stats = engine.get_can_stats();
    size_t idx = 0;
    for (const auto& [id, cs] : can_stats) {
        file << "    \"0x" << std::hex << id << std::dec << "\": {"
             << "\"count\": " << cs.count
             << ", \"avg_interval_ms\": " << std::setprecision(2) << cs.avg_interval_ms
             << "}" << (++idx < can_stats.size() ? "," : "") << "\n";
    }
    file << "  }\n}\n";
    return true;
}
