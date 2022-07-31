#include "spider_task.h"
namespace spider {
SpiderTask::SpiderTask(cpr::Url url, const std::string& save_directory) : spider_url_(url), save_directory_(save_directory) {};
// 初始化extractor
// std::unique_ptr<Extractor> extractor_ = std::make_unique<Extractor>();
// Download* download_ = new DownLoad();
// 构建download
SpiderTask::~SpiderTask(){
//  析构函数


}
}  // namespace spider
