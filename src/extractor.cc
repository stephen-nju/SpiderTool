#include "extractor.h"

#include <regex>

#include "absl/strings/string_view.h"
#include "utils.h"
namespace spider {

Extractor::Extractor(){};
Extractor::~Extractor(){};
void Extractor::parser_url(const absl::string_view& url) {
    // 解析url 判断网页视频类型
    return;
};

std::unique_ptr<ExtractOutput> Extractor::get_response() {
    cpr::Response r = cpr::Get(this->url,
                               cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
                               cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    printf("%s", r.text.c_str());
    //获取网页元素
    absl::string_view title = "data";
    // 解析元素
    std::unique_ptr<ExtractOutput> out =
        std::unique_ptr<ExtractOutput>(new ExtractOutput(VideoType::Normal, url, 10, title));
    return out;
};

void Extractor::parse_reponse(){};

std::unique_ptr<ExtractOutput> Extractor::get_extract_output() {
    absl::string_view title = "data";
    std::unique_ptr<ExtractOutput> out =
        std::unique_ptr<ExtractOutput>(new ExtractOutput(VideoType::Normal, url, 10, title));
    return out;
};
}  // namespace spider