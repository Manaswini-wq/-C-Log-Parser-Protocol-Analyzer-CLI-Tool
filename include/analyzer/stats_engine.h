#ifndef STATS_ENGINE_H
#define STATS_ENGINE_H

#include "parser/log_entry.h"
#include <vector>
#include <map>
#include <string>

struct ProtocolStats {
    size_t total_packets;
    double first_timestamp;
    double last_timestamp;
    double packets_per_second;
};

struct CANIDStats {
    uint32_t id;
    size_t   count;
    double   avg_interval_ms;
    double   min_interval_ms;
    double   max_interval_ms;
    size_t   total_bytes;
};

class StatsEngine {
public:
    void analyze(const std::vector<LogEntry>& entries);

    ProtocolStats get_overall_stats() const;
    std::map<uint32_t, CANIDStats> get_can_stats() const;
    std::map<ProtocolType, size_t> get_protocol_counts() const;
    std::vector<std::pair<double, double>> get_throughput_timeline(
        double bucket_sec = 1.0) const;

private:
    std::vector<LogEntry> entries_;
    ProtocolStats overall_;
    std::map<uint32_t, CANIDStats> can_stats_;
    std::map<ProtocolType, size_t> protocol_counts_;

    void compute_overall();
    void compute_can_stats();
    void compute_protocol_counts();
};

#endif
