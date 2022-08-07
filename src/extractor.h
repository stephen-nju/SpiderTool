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
    Ugc,
    Series,
    Live,
};

struct VideoInfo {
    VideoType type;
    std::unique_ptr<std::string> title;
    int bvid;
    int aid;
    int cid;
    int mid;
    VideoInfo() = default;
    virtual ~VideoInfo() = default;
};

struct VideoContent {
    int duration;
    std::unique_ptr<std::string> part;
};
struct UgcVideoInfo: public VideoInfo{

    std::list<std::unique_ptr<VideoContent>> items;
};
class Extractor {
private:
    std::unique_ptr<VideoInfo> video_info_;
    std::unique_ptr<cpr::Response> response_;
    bool parse_url(absl::string_view url);
    bool parse_ugc_response();
    bool parse_pgc_response();
    bool parse_pugv_response();

public:
    std::unique_ptr<VideoInfo> get_video_info();
    bool init(absl::string_view s);
    explicit Extractor();
    virtual ~Extractor();
};
}  // namespace spider
#endif