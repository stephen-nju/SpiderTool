#include "utils.h"
namespace spider {
std::string trim(const std::string& s) {
    auto start = s.begin();
    auto end = s.end() - 1;
    while (start < end) {
        if (std::isspace(*start)) {
            start++;
        } else if (std::isspace(*end)) {
            end--;
        } else {
            break;
        }
    }
    return std::string(start, end + 1);
};

}  // namespace spider