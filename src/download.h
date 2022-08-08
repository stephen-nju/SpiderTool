#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include "absl/strings/string_view.h"
#include "cpr/cpr.h"
#include "extractor.h"
namespace spider {

struct DownloadInfo {
    /* data */
};

class DownloadTask {
public:
    static std::unique_ptr<DownloadTask> from_videoinfo(VideoInfo* video_info);
    virtual std::string get_title() = 0;
    virtual void start_download(absl::string_view save_directory) = 0;
    DownloadTask();
    virtual ~DownloadTask();

protected:
    // 文件路径
    std::unique_ptr<std::string> path_;
    std::unique_ptr<cpr::Response> response_;
};

class UgcVideoDownloadTask : public DownloadTask {
protected:
    int total_video_size;
    std::unique_ptr<std::FILE> file_;
    std::unique_ptr<VideoInfo> video_info_;

public:
    bool parse_play_info();
    std::string get_title() override;
    void start_download(absl::string_view save_directory) override;
    UgcVideoDownloadTask(VideoInfo* video_info);
    UgcVideoDownloadTask()=delete;

    virtual ~UgcVideoDownloadTask();
};

}  // namespace spider
#endif