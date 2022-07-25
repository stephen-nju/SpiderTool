#ifndef SRC_EXTRACTOR_H
#define SRC_EXTRACTOR_H
#include <memory>
#include <regex>
#include <string>

#include "cpr/cpr.h"
#include "spdlog/spdlog.h"
namespace spider {
// 视频类型
enum class VideoType {
    Normal,
    Series,
    Live,
};

struct ExtractOutput {
    cpr::Url url;
    int id;
    VideoType type;
    std::string title;
    ExtractOutput(VideoType type, cpr::Url url, int id, std::string title)
        : type(type),
          url(url),
          id(id),
          title(title){};

    virtual ~ExtractOutput();
};

class Extractor {
private:
    void _not_supported_url(std::string url);
    std::unique_ptr<ExtractOutput> output;

public:
    bool parser_url(const std::string& url);
    std::unique_ptr<ExtractOutput> get_output();
    Extractor() = default;
    ~Extractor() = default;
};
}  // namespace spider
#endif