#include "extractor.h"

#include <iostream>
#include <regex>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "utils.h"

namespace ContentItemFlag {
constexpr int NoFlags = 0;
constexpr int Disabled = 1;
constexpr int VipOnly = 2;
constexpr int PayOnly = 4;
constexpr int AllowWaitFree = 8;  // manga
}  // namespace ContentItemFlag

namespace spider {

Extractor::Extractor() {
    // 初始化输出
    video_info_ = absl::make_unique<VideoInfo>();
    video_info_->title = absl::make_unique<std::string>();
};
Extractor::~Extractor(){};

bool Extractor::init(absl::string_view s) {
    if (this->parse_url(s)) {
    };
    return false;
}

bool Extractor::parse_url(absl::string_view url) {
    // 解析url 判断网页视频类型
    // 删除包含的http:// https:// 前缀
    absl::string_view url_s = absl::StripPrefix(url, "https://");
    absl::string_view url_s1 = absl::StripPrefix(url_s, "http://");
    absl::string_view url_ = spider::strip(url_s1);

    if (std::regex_match(url_.data(), std::regex("^(?:www\\.|m\\.)?bilibili\\.com.*$"))) {
        std::cmatch match;
        if (std::regex_match(url_.data(), match, std::regex("(?:.*BV|bv)([a-zA-Z0-9]+).*"))) {
            // 注意查看查看内存地址
            std::string api = absl::StrCat("https://api.bilibili.com/x/web-interface/view?bvid=", match.str(1).c_str());
            response_ = absl::make_unique<cpr::Response>(cpr::Get(cpr::Url(api.data())));
            printf_s("%s\n", api.c_str());
            if (parse_ugc_response()) {
                return true;
            }
        }
    }
    return false;
};

bool Extractor::parse_ugc_response() {
    // 解析网页html 获取标题等

    // 构建UGC video info
    rapidjson::Document document;
    document.Parse(response_->text.c_str());
    if (document.HasMember("data")) {
        rapidjson::Value& data = document["data"];
        if (data.HasMember("title")) {
            video_info_->title = std::move(absl::make_unique<std::string>(data["title"].GetString()));
        }
    }
    return false;
};

std::unique_ptr<VideoInfo> Extractor::get_video_info() {
    return std::move(this->video_info_);
};
}  // namespace spider