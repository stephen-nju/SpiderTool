#ifndef SRC_DOWNLOAD_H
#define SRC_DOWNLOAD_H
#include <cstdio>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

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

class DownloadTask {
public:
    // 初始化
    struct DownloadInfo {
        int video_quality;
        std::string video_format;
        std::string durl;
        bool dowload_finished = false;
        bool merge_finished = false;
        std::string tmp_dir;
        DownloadInfo();
        virtual ~DownloadInfo();
    };
    struct UgcDownloadInfo : DownloadInfo {
        // 合并分段下载视频合成顺序
        std::unordered_map<int, std::string> index_video_map_;
        UgcDownloadInfo() : DownloadInfo(){};
    };
    static std::unique_ptr<DownloadTask> from_videoinfo(std::unique_ptr<VideoInfo> video_info);
    bool run(absl::string_view save_directory);
    virtual std::string get_title() = 0;
    virtual bool start_download() = 0;
    // 纯虚函数
    virtual bool download_video(absl::string_view save_direcotry) = 0;
    virtual bool end_download() = 0;
    DownloadTask();
    virtual ~DownloadTask();

protected:
    // 视频下载的信息
    std::unique_ptr<cpr::Response> response_;
};

class UgcVideoDownloadTask : public DownloadTask {
protected:
    char video_path_[128];
    std::unique_ptr<VideoInfo> video_info_;
    std::unique_ptr<UgcDownloadInfo> download_info_;

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