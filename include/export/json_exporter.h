#ifndef JSON_EXPORTER_H
#define JSON_EXPORTER_H

#include "parser/log_entry.h"
#include "analyzer/stats_engine.h"
#include <vector>
#include <string>

class JSONExporter {
public:
    static bool export_entries(const std::string& filepath,
                               const std::vector<LogEntry>& entries);
    static bool export_stats(const std::string& filepath,
                             const StatsEngine& engine);
};

#endif
