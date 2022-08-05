#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstring>
#include <string>

namespace spider {

// 数据下载模块
class DownloadTask {
public:
    virtual void start_download() = 0;
    virtual void stop_download() = 0;
    DownloadTask();
    ~DownloadTask();
};

}  // namespace spider
#endif