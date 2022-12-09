#ifndef SRC_SPIDER_H
#define SRC_SPIDER_H
#include <cstdio>
#include <memory>
#include <string>

#include "absl/strings/string_view.h"
#include "cpr/cpr.h"
#include "download.h"
#include "extractor.h"
#include "task_pool.h"

namespace spider {
// 爬虫主任务
class SpiderTask {
private:
    absl::string_view spider_url_;
    absl::string_view save_directory_;
    std::unique_ptr<Extractor> extractor_;
    std::unique_ptr<DownloadTask> download_;

public:
    explicit SpiderTask(absl::string_view url, absl::string_view save_diretory);
    ~SpiderTask();
    void run();
};
}  // namespace spider
#endif