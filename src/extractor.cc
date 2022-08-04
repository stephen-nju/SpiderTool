#include "extractor.h"

#include <iostream>
#include <regex>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "utils.h"
namespace spider {

Extractor::Extractor() {
    // 初始化输出
    output_ = absl::make_unique<ExtractOutput>();
    output_->query = absl::make_unique<std::string>();
    output_->title = absl::make_unique<std::string>();
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
    // 解析url 判断网页视频类型
    // 删除包含的http:// https:// 前缀
    absl::string_view url_s = absl::StripPrefix(url, "https://");
    absl::string_view url_s1 = absl::StripPrefix(url_s, "http://");
    absl::string_view url_s2 = spider::strip(url_s1);
    url_ = absl::make_unique<absl::string_view>(url_s2.data(), url_s2.size());

    if (std::regex_match(url_->data(), std::regex("^(?:www\\.|m\\.)?bilibili\\.com.*$"))) {
        std::cmatch match;
        if (std::regex_match(url_->data(), match, std::regex("(?:.*BV|bv)([a-zA-Z0-9]+).*"))) {
            std::string concat =
                absl::StrCat("https://api.bilibili.com/x/web-interface/view?bvid=", match.str(1).c_str());
            // 查看内存地址
            std::unique_ptr<std::string> query = absl::make_unique<std::string>(concat);
            output_->query = std::move(query);
            return true;
        }
    }
    return false;
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