#include "extractor.h"

#include <list>
#include <memory>
#include <regex>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "spdlog/spdlog.h"
#include "utils.h"

namespace ContentItemFlag {
constexpr int NoFlags = 0;
constexpr int Disabled = 1;
constexpr int VipOnly = 2;
constexpr int PayOnly = 4;
constexpr int AllowWaitFree = 8;  // manga
}  // namespace ContentItemFlag

namespace spider {

VideoInfo::VideoInfo(){};

VideoInfo::~VideoInfo() {
    spdlog::info("runing VideoInfo Deconstruction");
};
UgcVideoInfo::UgcVideoInfo() : VideoInfo(){};

UgcVideoInfo::~UgcVideoInfo() {
    spdlog::info("runing UgcVideoInfo Deconstruction");
};

Extractor::Extractor(){};
Extractor::~Extractor() {
    spdlog::info("runing Extractor Deconstruction");
};

bool Extractor::init(absl::string_view s) {
    if (this->parse_url(s)) {
        return true;
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
            response_ = std::make_unique<cpr::Response>(cpr::Get(cpr::Url(api.data())));
            spdlog::info(api.c_str());
            if (parse_ugc_response()) {
                return true;
            }
        }
    }
    return false;
};

bool Extractor::parse_ugc_response() {
    // 解析网页html 获取标题等std

    // 构建UGC video info
    rapidjson::Document document;
    auto ugc_video_info = std::make_unique<UgcVideoInfo>();
    ugc_video_info->type = VideoType::Ugc;
    document.Parse(response_->text.c_str());
    // spdlog::info(response_->text.c_str());
    if (document.HasMember("data")) {
        rapidjson::Value& data = document["data"];
        if (!data.HasMember("title")) {
            return false;
        }
        if (!data.HasMember("aid")) {
            return false;
        }
        if (!data.HasMember("cid")) {
            return false;
        }
        ugc_video_info->title = std::make_unique<std::string>(data["title"].GetString());
        ugc_video_info->aid = data["aid"].GetInt();
        ugc_video_info->cid = data["cid"].GetInt();

        // 获取owner 的mid
        if (data.HasMember("owner")) {
            rapidjson::Value& owner = data["owner"];
            if (!owner.HasMember("mid")) {
                spdlog::error("response doesnot have mid");
                return false;
            }
            ugc_video_info->mid = owner["mid"].GetInt();
        } else {
            return false;
        }
        if (data.HasMember("pages")) {
            rapidjson::Value& array = data["pages"].GetArray();
            for (rapidjson::SizeType i = 0; i < array.Size(); i++) {
                std::unique_ptr<VideoContent> content = std::make_unique<VideoContent>();

                rapidjson::Value& value = array[i];
                if (!value.HasMember("duration")) {
                    return false;
                }
                if (!value.HasMember("part")) {
                    return false;
                }
                int duration = value["duration"].GetInt();
                content->duration = duration;
                content->part = std::make_unique<std::string>(value["part"].GetString());
                ugc_video_info->items.emplace_back(std::move(content));
            }
            video_info_ = std::move(ugc_video_info);
            return true;
        }

        return true;
    }
    return false;
};

std::unique_ptr<VideoInfo> Extractor::get_video_info() {
    return std::move(this->video_info_);
};
}  // namespace spider