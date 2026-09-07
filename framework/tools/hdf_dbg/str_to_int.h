#ifndef HDF_DBG_STR_TO_INT_H
#define HDF_DBG_STR_TO_INT_H

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <string>
#include <system_error>

inline int ParseDbgInt(const std::string &tempstr, int32_t fallback)
{
    if (tempstr.empty() || !std::all_of(tempstr.begin(), tempstr.end(), ::isdigit)) {
        return fallback;
    }
    int32_t parsed = 0;
    const char *first = tempstr.data();
    const char *last = first + tempstr.size();
    auto result = std::from_chars(first, last, parsed);
    if (result.ec == std::errc() && result.ptr == last) {
        return parsed;
    }
    return fallback;
}

#endif
