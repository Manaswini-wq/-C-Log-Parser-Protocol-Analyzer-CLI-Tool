#include "utils/timestamp.h"
#include <sstream>
#include <iomanip>
#include <cmath>

std::string format_timestamp(double seconds) {
    int hrs = static_cast<int>(seconds) / 3600;
    int min = (static_cast<int>(seconds) % 3600) / 60;
    double sec = fmod(seconds, 60.0);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hrs << ":"
        << std::setw(2) << min << ":"
        << std::fixed << std::setprecision(3) << std::setw(6) << sec;
    return oss.str();
}

std::string format_duration(double seconds) {
    if (seconds < 1.0) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << (seconds * 1000.0) << " ms";
        return oss.str();
    }
    return format_timestamp(seconds);
}
