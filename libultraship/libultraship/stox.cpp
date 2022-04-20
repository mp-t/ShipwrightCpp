#include "stox.h"
#include "spdlog/spdlog.h"

namespace Ship {
    bool stob(const std::string& s, bool defaultVal) {
        auto result = false;    // failure to assert is false

        std::istringstream is(s);
        // first try simple integer conversion
        is >> result;

        if (is.fail()){
            // simple integer failed; try boolean
            is.clear();
            is >> std::boolalpha >> result;
        }

        if (is.fail()) {
            SPDLOG_DEBUG("Failed to parse boolean from string: {}", s);
            return defaultVal;
        }

        return result;
    }

    std::int32_t stoi(const std::string& s, std::int32_t defaultVal) {
        try {
            return std::stoi(s);
        } catch ([[maybe_unused]] const std::invalid_argument& e) {
            SPDLOG_DEBUG("Failed to parse std::int32_t from string due to invalid argument: {} ({})", e.what(), s);
        } catch ([[maybe_unused]] const std::out_of_range& e) {
            SPDLOG_DEBUG("Failed to parse std::int32_t from string due to out of range: {} ({})", e.what(), s);
        }

        return defaultVal;
    }

    float stof(const std::string& s, float defaultVal) {
        try {
            return std::stof(s);
        }
        catch ([[maybe_unused]] const std::invalid_argument& e) {
            SPDLOG_DEBUG("Failed to parse float from string due to invalid argument: {} ({})", e.what(), s);
        }
        catch ([[maybe_unused]] const std::out_of_range& e) {
            SPDLOG_DEBUG("Failed to parse float from string due to out of range: {} ({})", e.what(), s);
        }

        return defaultVal;
    }

    std::int64_t stoll(const std::string& s, std::int64_t defaultVal) {
        try {
            return std::stoll(s);
        } catch ([[maybe_unused]] const std::invalid_argument& e) {
            SPDLOG_DEBUG("Failed to parse std::int32_t from string due to invalid argument: {} ({})", e.what(), s);
        } catch ([[maybe_unused]] const std::out_of_range& e) {
            SPDLOG_DEBUG("Failed to parse std::int32_t from string due to out of range: {} ({})", e.what(), s);
        }

        return defaultVal;
    }
}