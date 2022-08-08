#include "download.h"

#include <cstdio>
#include <filesystem>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "rapidjson/document.h"
namespace fs = std::filesystem;

namespace spider {
DownloadTask::DownloadTask(){};
DownloadTask::~DownloadTask(){};

UgcVideoDownloadTask::UgcVideoDownloadTask(VideoInfo* video_info) : DownloadTask(), video_info_(video_info) {
    char api[512];
    sprintf(api,
            "https://api.bilibili.com/x/player/playurl?avid=%d&cid=%d&qn=2&fourk=1",
            video_info_->aid,
            video_info_->cid);

    printf("%s\n",api);
    response_ = absl::make_unique<cpr::Response>(cpr::Get(cpr::Url(api)));
};

std::unique_ptr<DownloadTask> DownloadTask::from_videoinfo(VideoInfo* video_info) {
    VideoType type = video_info->type;
    switch (type) {
    case VideoType::Ugc:
        return absl::make_unique<UgcVideoDownloadTask>(video_info);
    }
    return nullptr;
}

std::string UgcVideoDownloadTask::get_title() {
    return std::string("data");
};

bool UgcVideoDownloadTask::parse_play_info() {
    rapidjson::Document document;
    document.Parse(response_->text.c_str());
    printf("%s\n", response_->text.c_str());
    return true;
}

void UgcVideoDownloadTask::start_download(absl::string_view save_directory) {
    if (parse_play_info()) {
        // 解析视频地址，视频后缀等
        fs::path path(save_directory);
        if (!fs::exists(path)) {
            if (std::filesystem::create_directory(save_directory.data())) {
                std::string file_path = absl::StrCat(save_directory, this->get_title());
                file_ = std::unique_ptr<std::FILE>(fopen(file_path.c_str(), "w+"));
                if (file_.get() != nullptr) {
                }
            }
            // 创建下载目录
        };
    };
};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider