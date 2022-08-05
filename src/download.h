#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include "cpr/cpr.h"
namespace spider {

// 数据下载模块
class DownloadTask {
public:
    static DownloadTask* from_videoinfo();
    virtual void start_download() = 0;
    virtual std::string get_title();
    virtual ~DownloadTask();

protected:
    // 文件路径
    std::unique_ptr<std::string> path_;
    std::unique_ptr<cpr::Response> response_;
    DownloadTask();

public:
    std::string get_path() {
        return *path_;
    };
};

class UgcVideoDownloadTask : public DownloadTask {
protected:
    int totol_video_size;
    std::unique_ptr<std::FILE> file_;

public:
    void parse_url();
    std::string get_title() override;
    void start_download() override;
};

}  // namespace spider
#endif