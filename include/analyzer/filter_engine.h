#ifndef FILTER_ENGINE_H
#define FILTER_ENGINE_H

#include "parser/log_entry.h"
#include <vector>
#include <functional>
#include <set>

class FilterEngine {
public:
    // Filter by protocol type
    FilterEngine& protocol(ProtocolType type);

    // Filter by CAN ID (single or set)
    FilterEngine& can_id(uint32_t id);
    FilterEngine& can_ids(const std::set<uint32_t>& ids);

    // Filter by time range
    FilterEngine& time_range(double start_sec, double end_sec);

    // Filter by data content (byte at position equals value)
    FilterEngine& data_byte(size_t position, uint8_t value);

    // Custom filter
    FilterEngine& custom(std::function<bool(const LogEntry&)> predicate);

    // Apply all filters and return matching entries
    std::vector<LogEntry> apply(const std::vector<LogEntry>& entries) const;

    // Reset all filters
    void clear();

private:
    std::vector<std::function<bool(const LogEntry&)>> filters_;
};

#endif
