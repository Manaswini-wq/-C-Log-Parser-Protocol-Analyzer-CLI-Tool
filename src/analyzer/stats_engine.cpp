#include "analyzer/stats_engine.h"
#include <algorithm>
#include <cmath>
#include <limits>

void StatsEngine::analyze(const std::vector<LogEntry>& entries) {
    entries_ = entries;
    compute_overall();
    compute_can_stats();
    compute_protocol_counts();
}

void StatsEngine::compute_overall() {
    overall_ = {};
    overall_.total_packets = entries_.size();
    if (entries_.empty()) return;

    overall_.first_timestamp = entries_.front().timestamp_sec;
    overall_.last_timestamp = entries_.back().timestamp_sec;
    double duration = overall_.last_timestamp - overall_.first_timestamp;
    overall_.packets_per_second = (duration > 0) ?
        (double)overall_.total_packets / duration : 0;
}

void StatsEngine::compute_can_stats() {
    can_stats_.clear();
    std::map<uint32_t, std::vector<double>> timestamps;

    for (const auto& e : entries_) {
        if (e.protocol != ProtocolType::CAN) continue;
        timestamps[e.can_id].push_back(e.timestamp_sec);
    }

    for (auto& [id, ts_vec] : timestamps) {
        CANIDStats stats = {};
        stats.id = id;
        stats.count = ts_vec.size();
        stats.total_bytes = stats.count * 8;  // approximate

        if (ts_vec.size() >= 2) {
            double sum_interval = 0;
            stats.min_interval_ms = std::numeric_limits<double>::max();
            stats.max_interval_ms = 0;

            for (size_t i = 1; i < ts_vec.size(); i++) {
                double interval = (ts_vec[i] - ts_vec[i-1]) * 1000.0;
                sum_interval += interval;
                stats.min_interval_ms = std::min(stats.min_interval_ms, interval);
                stats.max_interval_ms = std::max(stats.max_interval_ms, interval);
            }
            stats.avg_interval_ms = sum_interval / (double)(ts_vec.size() - 1);
        }
        can_stats_[id] = stats;
    }
}

void StatsEngine::compute_protocol_counts() {
    protocol_counts_.clear();
    for (const auto& e : entries_) {
        protocol_counts_[e.protocol]++;
    }
}

ProtocolStats StatsEngine::get_overall_stats() const { return overall_; }
std::map<uint32_t, CANIDStats> StatsEngine::get_can_stats() const { return can_stats_; }
std::map<ProtocolType, size_t> StatsEngine::get_protocol_counts() const { return protocol_counts_; }

std::vector<std::pair<double, double>> StatsEngine::get_throughput_timeline(
        double bucket_sec) const {
    std::vector<std::pair<double, double>> timeline;
    if (entries_.empty()) return timeline;

    double start = entries_.front().timestamp_sec;
    double end = entries_.back().timestamp_sec;
    size_t idx = 0;

    for (double t = start; t <= end; t += bucket_sec) {
        size_t count = 0;
        while (idx < entries_.size() && entries_[idx].timestamp_sec < t + bucket_sec) {
            count++;
            idx++;
        }
        timeline.push_back({t, (double)count / bucket_sec});
    }
    return timeline;
}
