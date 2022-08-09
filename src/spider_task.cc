#include "spider_task.h"

#include "absl/memory/memory.h"
#include "download.h"
#include "extractor.h"
namespace spider {
SpiderTask::SpiderTask(absl::string_view url, const absl::string_view& save_directory)
    : spider_url_(url),
      save_directory_(save_directory) {
    extractor_ = absl::make_unique<Extractor>();
};
// 构建download
SpiderTask::~SpiderTask(){};

void SpiderTask::run() {
    if (extractor_->init(spider_url_)) {
        std::unique_ptr<VideoInfo> video_info = extractor_->get_video_info();
        download_ = DownloadTask::from_videoinfo(video_info.get());
        if (download_ != nullptr) {
            download_->start_download(save_directory_);
        }
    };
};
}  // namespace spider
