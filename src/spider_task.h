#ifndef SRC_SPIDER_H
#define SRC_SPIDER_H
#include <cstdio>
#include <memory>
#include <string>

#include "cpr/cpr.h"
#include "download.h"
#include "extractor.h"
#include "task_pool.h"

namespace spider {
// 爬虫主任务
class SpiderTask {
private:
    cpr::Url spider_url_;
    std::string save_directory_;
    std::string video_name_;
    std::unique_ptr<Extractor> extractor_;
    Download* download_;
    std::string get_title();

public:
    explicit SpiderTask(cpr::Url url, const std::string& save_diretory);
    ~SpiderTask();
    bool run();
};
}  // namespace spider
#endif