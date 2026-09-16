#include "export/csv_exporter.h"
#include <fstream>
#include <iomanip>
#include <sstream>

bool CSVExporter::export_entries(const std::string& filepath,
                                 const std::vector<LogEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "timestamp,protocol,can_id,can_dlc,uart_port,uart_dir,data_hex\n";

    for (const auto& e : entries) {
        file << std::fixed << std::setprecision(6) << e.timestamp_sec << ",";
        file << (e.protocol == ProtocolType::CAN ? "CAN" : "UART") << ",";

        if (e.protocol == ProtocolType::CAN) {
            file << "0x" << std::hex << std::setw(3) << std::setfill('0') << e.can_id
                 << std::dec << "," << (int)e.can_dlc << ",,";
        } else {
            file << ",," << (int)e.uart_port << "," << e.uart_direction;
        }

        file << ",";
        const auto& data = (e.protocol == ProtocolType::CAN) ? e.can_data : e.uart_data;
        for (size_t i = 0; i < data.size(); i++) {
            if (i > 0) file << " ";
            file << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
        }
        file << std::dec << "\n";
    }
    return true;
}

bool CSVExporter::export_decoded(const std::string& filepath,
                                  const std::vector<DecodedPacket>& packets) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "protocol,message,field,value,unit\n";
    for (const auto& pkt : packets) {
        for (const auto& f : pkt.fields) {
            file << pkt.protocol_name << "," << pkt.message_name << ","
                 << f.name << "," << f.value << "," << f.unit << "\n";
        }
    }
    return true;
}
