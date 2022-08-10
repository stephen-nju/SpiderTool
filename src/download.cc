#include "download.h"

#include <cstdio>
#include <filesystem>
#include <stdexcept>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
namespace fs = std::filesystem;

namespace spider {

DownloadInfo::DownloadInfo(){};
DownloadInfo::~DownloadInfo(){};

DownloadTask::DownloadTask(){};
DownloadTask::~DownloadTask() {
    spdlog::info("runing DownloadTask Deconstruction");
};

UgcVideoDownloadTask::UgcVideoDownloadTask(std::unique_ptr<VideoInfo> video_info) : DownloadTask() {
    video_info_ = std::move(video_info);
    download_info_ = std::make_unique<DownloadInfo>();
    // download_info_->durl=std::list<std::unique_ptr<Section>> list;
};

std::unique_ptr<DownloadTask> DownloadTask::from_videoinfo(std::unique_ptr<VideoInfo> video_info) {
    VideoType type = video_info->type;
    switch (type) {
    case VideoType::Ugc:
        return absl::make_unique<UgcVideoDownloadTask>(std::move(video_info));
    }
    return nullptr;
}

std::string UgcVideoDownloadTask::get_title() {
    return *video_info_->title;
};

bool UgcVideoDownloadTask::parse_play_info() {
    char api[512];
    int n = sprintf(api,
                    "https://api.bilibili.com/x/player/playurl?avid=%d&cid=%d&qn=64&fourk=1",
                    video_info_->aid,
                    video_info_->cid);

    if (n > 0) {
        printf("%s\n", api);
        cpr::Response response = cpr::Get(cpr::Url(api));
        rapidjson::Document document;
        document.Parse(response.text.c_str());
        if (document.HasMember("data")) {
            rapidjson::Value& data = document["data"];
            if (data.HasMember("durl")) {
                rapidjson::Value& durl = data["durl"].GetArray();
                for (rapidjson::SizeType i = 0; i < durl.Size(); i++) {
                    rapidjson::Value& value = durl[i];
                    std::unique_ptr<Section> s = std::make_unique<Section>();
                    if (!value.HasMember("order")) {
                        return false;
                    }
                    if (!value.HasMember("url")) {
                        return false;
                    }
                    s->order = value["order"].GetInt();
                    s->section_url = value["url"].GetString();
                    // 更新downloadinfo信息
                    download_info_->durl.emplace_back(std::move(s));
                }

                return true;
            }
        }
    }

    return false;
};

void UgcVideoDownloadTask::start_download(absl::string_view save_directory) {
    if (parse_play_info()) {
        // 解析视频地址，视频后缀等
        fs::path path(save_directory);
        if (!fs::exists(path)) {
            if (!std::filesystem::create_directory(save_directory.data())) {
                throw std::runtime_error("create saved directory error");
            }
        }
        for (std::list<std::unique_ptr<Section>>::iterator it = download_info_->durl.begin();
             it != download_info_->durl.end();
             it++) {
            int order = (*it)->order;
            std::string url = (*it)->section_url;
            spdlog::info("{}\n", url.c_str());
            // 分段视频需要后期合并
        }
    }
};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider