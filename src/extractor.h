#ifndef SRC_EXTRACTOR_H
#define SRC_EXTRACTOR_H
#include <memory>
#include <regex>
#include <string>

#include "cpr/cpr.h"
#include "absl/strings/string_view.h"

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
    absl::string_view title;
    ExtractOutput(VideoType type, cpr::Url url, int id, absl::string_view title)
        : type(type),
          url(url),
          id(id),
          title(title){};
    virtual ~ExtractOutput()=default;
};

class Extractor {
private:
    cpr::Url url;
    bool _not_supported_url(absl::string_view url);
public:
    void parser_url(const absl::string_view& url);
    void parse_reponse();
    std::unique_ptr<ExtractOutput> get_response();
    std::unique_ptr<ExtractOutput> get_extract_output();

    explicit Extractor() ;
   virtual  ~Extractor();
};
}  // namespace spider
#endif