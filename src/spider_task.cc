#include "spider_task.h"

#include "absl/memory/memory.h"
namespace spider {
SpiderTask::SpiderTask(absl::string_view url, const absl::string_view& save_directory)
    : spider_url_(url),
      save_directory_(save_directory) {
    extractor_ = absl::make_unique<Extractor>();
};
// 初始化extractor

// Download* download_ = new DownLoad();
// 构建download
SpiderTask::~SpiderTask(){
    //  析构函数
};

void SpiderTask::run() {
    if (!extractor_->init(spider_url_)) {
        printf(u8"输入格式不支持\n");
    };
};
}  // namespace spider
