#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstdio>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "absl/strings/string_view.h"
#include "cpr/cpr.h"
#include "extractor.h"
namespace spider {

// 视频清晰度
static std::map<int, std::string> quality_map{
    {120, "4K 超清"},
    {116, "1080P 60帧"},
    {112, "1080P 高码率"},
    {80, "1080P 高清"},
    {74, "720P 60帧"},
    {64, "720P 高清"},
    {32, "480P 清晰"},
    {16, "360P 流畅"},
};

struct Section {
    // 用于下载系列影片
    int order;
    std::string section_url;
};
struct DownloadInfo {
    std::string video_name;
    // download video format mp4 flv .....
    std::string video_format;
    // download video url .......
    std::list<std::unique_ptr<Section>> durl;
    DownloadInfo();
    ~DownloadInfo();
};

class DownloadTask {
public:
    static std::unique_ptr<DownloadTask> from_videoinfo(std::unique_ptr<VideoInfo> video_info);
    virtual std::string get_title() = 0;
    virtual bool start_download() = 0;
    // 纯虚函数
    virtual bool download_video(absl::string_view save_direcotry) = 0;
    virtual bool end_download() = 0;
    DownloadTask();
    virtual ~DownloadTask();

protected:
    // 文件路径
    std::unique_ptr<DownloadInfo> download_info_;
    std::unique_ptr<std::string> path_;
    std::unique_ptr<cpr::Response> response_;
};

class UgcVideoDownloadTask : public DownloadTask {
protected:
    int total_video_size_;
    int video_download_quality_;
    std::unique_ptr<std::vector<int>> video_accept_quality_;
    std::unique_ptr<std::FILE> file_;
    std::unique_ptr<VideoInfo> video_info_;

public:
    bool parse_play_info();
    bool get_video_quality();
    std::string get_title() override;
    bool start_download() override;
    bool download_video(absl::string_view save_directory) override;
    bool end_download() override;
    UgcVideoDownloadTask(std::unique_ptr<VideoInfo> video_info);
    UgcVideoDownloadTask() = delete;

    virtual ~UgcVideoDownloadTask();
};

}  // namespace spider
#endif