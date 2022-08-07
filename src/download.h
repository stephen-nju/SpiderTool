#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include "cpr/cpr.h"
namespace spider {

class DownloadTask {
public:
    static std::unique_ptr<DownloadTask> from_videoinfo();
    virtual std::string get_title()=0;
    virtual void start_download()=0;
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
    UgcVideoDownloadTask();

public:
    void parse_url();
    std::string get_title() override;
    void start_download() override;
    ~UgcVideoDownloadTask();
};

}  // namespace spider
#endif