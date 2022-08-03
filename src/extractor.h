#ifndef SRC_EXTRACTOR_H
#define SRC_EXTRACTOR_H
#include <memory>
#include <regex>
#include <string>

#include "absl/strings/string_view.h"
#include "cpr/cpr.h"

namespace spider {
// 视频类型

enum class VideoType {
    Normal,
    Series,
    Live,
};

struct ExtractOutput {
    std::unique_ptr<absl::string_view> query;
    int id;
    VideoType type;
    std::string title;

    // ExtractOutput(VideoType type, cpr::Url url, int id, absl::string_view title)
    //     : type(type),
    //       url(url),
    //       id(id),
    //       title(title){};

    ExtractOutput() = default;
    virtual ~ExtractOutput() = default;
};

class Extractor {
private:
    std::unique_ptr<absl::string_view> url_;
    std::unique_ptr<ExtractOutput> output_;
    cpr::Response* response_ = nullptr;
    bool parser_url(absl::string_view url);
    bool parse_reponse();

public:
    std::unique_ptr<ExtractOutput> get_extract_output();
    bool init(absl::string_view s);
    explicit Extractor();
    virtual ~Extractor();
};
}  // namespace spider
#endif