#include "download.h"

#include "absl/memory/memory.h"

namespace spider {

DownloadTask::DownloadTask(){};
DownloadTask::~DownloadTask(){};
// std::unique_ptr<DownloadTask> DownloadTask::from_videoinfo() {
//     std::unique_ptr<DownloadTask> download_task = absl::make_unique<UgcVideoDownloadTask>();
//     return download_task;
// };


UgcVideoDownloadTask::UgcVideoDownloadTask():DownloadTask(){};


std::string UgcVideoDownloadTask::get_title() {
    return std::string("data");
};
void UgcVideoDownloadTask::start_download() {};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider