#include "analyzer/filter_engine.h"

FilterEngine& FilterEngine::protocol(ProtocolType type) {
    filters_.push_back([type](const LogEntry& e) { return e.protocol == type; });
    return *this;
}

FilterEngine& FilterEngine::can_id(uint32_t id) {
    filters_.push_back([id](const LogEntry& e) {
        return e.protocol == ProtocolType::CAN && e.can_id == id;
    });
    return *this;
}

FilterEngine& FilterEngine::can_ids(const std::set<uint32_t>& ids) {
    filters_.push_back([ids](const LogEntry& e) {
        return e.protocol == ProtocolType::CAN && ids.count(e.can_id) > 0;
    });
    return *this;
}

FilterEngine& FilterEngine::time_range(double start_sec, double end_sec) {
    filters_.push_back([start_sec, end_sec](const LogEntry& e) {
        return e.timestamp_sec >= start_sec && e.timestamp_sec <= end_sec;
    });
    return *this;
}

FilterEngine& FilterEngine::data_byte(size_t position, uint8_t value) {
    filters_.push_back([position, value](const LogEntry& e) {
        if (e.protocol == ProtocolType::CAN)
            return position < e.can_data.size() && e.can_data[position] == value;
        if (e.protocol == ProtocolType::UART)
            return position < e.uart_data.size() && e.uart_data[position] == value;
        return false;
    });
    return *this;
}

FilterEngine& FilterEngine::custom(std::function<bool(const LogEntry&)> predicate) {
    filters_.push_back(predicate);
    return *this;
}

std::vector<LogEntry> FilterEngine::apply(const std::vector<LogEntry>& entries) const {
    std::vector<LogEntry> result;
    for (const auto& e : entries) {
        bool pass = true;
        for (const auto& f : filters_) {
            if (!f(e)) { pass = false; break; }
        }
        if (pass) result.push_back(e);
    }
    return result;
}

void FilterEngine::clear() {
    filters_.clear();
}
