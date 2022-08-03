#include "extractor.h"

#include <iostream>
#include <regex>

#include "absl/memory/memory.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "utils.h"
namespace spider {

Extractor::Extractor() {
    output_ = absl::make_unique<ExtractOutput>();
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
    // 删除包含的http:// https:// 前缀
    absl::string_view url_s = absl::StripPrefix(url_trim, "https://");
    absl::string_view url_s1 = absl::StripPrefix(url_s, "http://");
    url_ = absl::make_unique<absl::string_view>(url_s1.data());
    if (std::regex_match(url_s1.data(), std::regex(u8"^(?:www\.|m\.)?bilibili\.com.*$"))) {
        std::cmatch match;
        if (std::regex_search(url_s1.data(), match, std::regex(u8"/video/BV|bv([a-zA-Z0-9]+)?"))) {
            std::cout << match[1];
        }
    }
    return true;
};

bool Extractor::parse_reponse() {
    // 解析网页html 获取标题等
    cpr::Response r = cpr::Get(cpr::Url(url_->data()));
    printf("%s", r.text.c_str());
    return true;
};

std::unique_ptr<ExtractOutput> Extractor::get_extract_output() {
    return std::move(this->output_);
};
}  // namespace spider