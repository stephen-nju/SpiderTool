#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstring>
#include <string>

namespace spider {

// 数据下载模块
class Download {
protected:
    std::string path;

public:
    virtual void start_download() = 0;
    virtual void stop_download() = 0;
    Download();
    ~Download();
};

class VideoDownload : public Download {
public:
    void start_download() override;
    void stop_download() override;
};

}  // namespace spider
#endif