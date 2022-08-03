#ifndef SRC_UTILS_H
#define SRC_UTILS_H
#include "absl/strings/string_view.h"
namespace spider {

absl::string_view rstrip(absl::string_view s);

absl::string_view lstrip(absl::string_view s);

absl::string_view strip(absl::string_view s);

}  // namespace spider
#endif