#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "parser/log_entry.h"
#include "protocol/packet_decoder.h"
#include <vector>
#include <string>

class CSVExporter {
public:
    static bool export_entries(const std::string& filepath,
                               const std::vector<LogEntry>& entries);
    static bool export_decoded(const std::string& filepath,
                               const std::vector<DecodedPacket>& packets);
};

#endif
