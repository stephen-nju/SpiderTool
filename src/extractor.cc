#include "extractor.h"

#include <regex>

#include "absl/memory/memory.h"
#include "absl/strings/string_view.h"
#include "utils.h"
namespace spider {

Extractor::Extractor() {
    this->output_ = absl::make_unique<ExtractOutput>();
    this->url_ = absl::make_unique<absl::string_view>(nullptr);
};
Extractor::~Extractor(){};

bool Extractor::init(absl::string_view s) {
    if (this->parser_url(s)) {
        if (this->parse_reponse()) {
        };
    };
    return false;
}
bool Extractor::parser_url(absl::string_view url) {
    absl::string_view url_trim = spider::strip(url);
    // 解析url 判断网页视频类型
    this->url_ = absl::make_unique<absl::string_view>(url_trim.data());

    if (std::regex_match(url_trim.data(), std::regex("(?:www\.|m\.)?bilibili\.com"))) {
        if (std::regex_match(url_trim.data(), std::regex("(?:BV|bv)([a-zA-Z0-9]+)?/"))) {
            // UGC BV id

            return true;
        }
    }
    return true;
};

bool Extractor::parse_reponse() {
    // 解析网页html 获取标题等
    printf("%s\n", url_->data());
    cpr::Response r = cpr::Get(cpr::Url(url_->data()));
    printf("%s", r.text.c_str());
    return true;
};

std::unique_ptr<ExtractOutput> Extractor::get_extract_output() {
    return std::move(this->output_);
};
}  // namespace spider