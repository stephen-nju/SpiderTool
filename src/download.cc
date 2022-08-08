#include "download.h"

#include <cstdio>
#include <filesystem>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
namespace fs = std::filesystem;

namespace spider {
DownloadTask::DownloadTask(){};
DownloadTask::~DownloadTask(){};

UgcVideoDownloadTask::UgcVideoDownloadTask()
    : DownloadTask(){

      };

std::unique_ptr<DownloadTask> DownloadTask::from_videoinfo(VideoInfo* video_info) {
    VideoType type = video_info->type;
    switch (type) {
    case VideoType::Ugc:
        return absl::make_unique<UgcVideoDownloadTask>();
    }
    return nullptr;
}

std::string UgcVideoDownloadTask::get_title() {
    return std::string("data");
};

bool UgcVideoDownlaodTask::parse_url() {}

void UgcVideoDownloadTask::start_download(absl::string_view save_directory) {
    if (parse_url()) {
        // 解析视频地址，视频后缀等
        fs::path path(save_directory);
        if (!fs::exists(path)) {
            if (std::filesystem::create_directory(save_directory.data())) {
                std::string file_path = absl::StrCat(save_directory, this->get_title());
                file_ = absl::make_unique<std::FILE>(fopen(file_path.c_str(), "w+"));
                if (file_.get() != nullptr) {
                }
            }
            // 创建下载目录
        };
    };
};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider