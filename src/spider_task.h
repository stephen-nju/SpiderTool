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
    absl::string_view video_name_;
    std::unique_ptr<Extractor> extractor_;
    Download* download_;
    absl::string_view get_title();

public:
    explicit SpiderTask(absl::string_view url, const absl::string_view& save_diretory);
    ~SpiderTask();
    void run();
};
}  // namespace spider
#endif