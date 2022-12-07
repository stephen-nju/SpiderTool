#include "download.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "cpr/callback.h"
#include "cpr/cpr.h"
#include "cpr/cprtypes.h"
#include "cpr/session.h"
#include "omp.h"
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
namespace fs = std::filesystem;

namespace spider {
std::mutex mutex;

DownloadInfo::DownloadInfo(){};
DownloadInfo::~DownloadInfo(){};

DownloadTask::DownloadTask(){};
DownloadTask::~DownloadTask() {
    spdlog::info("runing DownloadTask Deconstruction");
};

UgcVideoDownloadTask::UgcVideoDownloadTask(std::unique_ptr<VideoInfo> video_info)
    : DownloadTask(),
      video_download_quality_(32) {
    video_info_ = std::move(video_info);
    download_info_ = std::make_unique<DownloadInfo>();
    // download_info_->durl=std::list<std::unique_ptr<Section>> list;
};

std::unique_ptr<DownloadTask> DownloadTask::from_videoinfo(std::unique_ptr<VideoInfo> video_info) {
    VideoType type = video_info->type;
    switch (type) {
    case VideoType::Ugc:
        return absl::make_unique<UgcVideoDownloadTask>(std::move(video_info));
    }
    return nullptr;
}

std::string UgcVideoDownloadTask::get_title() {
    return *video_info_->title;
};

bool UgcVideoDownloadTask::get_video_quality() {
    cpr::Header header = cpr::Header{
        {"User-Agent", "Mozilla/5.0"},
        {"Referer", "https://www.bilibili.com"},
    };
    cpr::Session session;
    char raw_api[512];
    int n = sprintf(raw_api,
                    "https://api.bilibili.com/x/player/playurl?avid=%d&cid=%d&qn=%d&fourk=1",
                    video_info_->aid,
                    video_info_->cid,
                    this->video_download_quality_);
    // 初始化下载质量为64，720p高清
    if (n > 0) {
        cpr::Url url = cpr::Url(raw_api);
        session.SetUrl(url);
        session.SetHeader(header);

        cpr::Response response = session.Get();
        spdlog::info(raw_api);

        rapidjson::Document doc;
        doc.Parse(response.text.c_str());
        if (!doc.HasMember("data")) {
            spdlog::error("parse video quality  url  error: {}", response.text.c_str());
            return false;
        }
        rapidjson::Value& data = doc["data"];
        if (!data.HasMember("accept_quality")) {
            spdlog::error("can not parse accept quality");
            return false;
        }
        if (!data.HasMember("support_formats")) {
            spdlog::warn("can not parse video format");
            return false;
        }
        rapidjson::Value& support_formats = data["support_formats"].GetArray();

        std::unique_ptr<std::unordered_map<int, std::string>> quality_format_map =
            std::make_unique<std::unordered_map<int, std::string>>();
        for (rapidjson::SizeType i = 0; i < support_formats.Size(); i++) {
            rapidjson::Value& formats = support_formats[i];
            rapidjson::Value& format_info = formats.GetObj();
            quality_format_map->emplace(format_info["quality"].GetInt(), format_info["format"].GetString());
        }

        rapidjson::Value& accept_quality = data["accept_quality"].GetArray();
        video_accept_quality_ = std::make_unique<std::vector<int>>();
        for (rapidjson::SizeType i = 0; i < accept_quality.Size(); i++) {
            rapidjson::Value& value = accept_quality[i];
            video_accept_quality_->push_back(value.GetInt());
        };
        spdlog::info("解析视频支持的分辨率:====================");
        for (auto q = video_accept_quality_->begin(); q != video_accept_quality_->end(); q++) {
            spdlog::info("分辨率:{0},编码:{1}\n", quality_map[*q], *q);
        }
        // spdlog::info("choose your video downlaod quality\n");
        // 优先选择最高分辨率
        // scanf("%d", &video_download_quality_);
        video_download_quality_ = video_accept_quality_->at(1);

        download_info_->video_format = quality_format_map->at(video_download_quality_);
        // 解析分辨率支持格式

        return true;
    }

    return false;
};

bool UgcVideoDownloadTask::parse_play_info() {
    char api[512];
    int n = sprintf(api,
                    "https://api.bilibili.com/x/player/playurl?avid=%d&cid=%d&qn=%d&fourk=1",
                    video_info_->aid,
                    video_info_->cid,
                    this->video_download_quality_);
    spdlog::info(api);

    if (n > 0) {
        cpr::Response response = cpr::Get(cpr::Url(api));
        rapidjson::Document document;
        spdlog::info(response.text.c_str());
        document.Parse(response.text.c_str());

        if (!document.HasMember("data")) {
            spdlog::warn("parse video play info error: {}", response.text.c_str());
            return false;
        }
        rapidjson::Value& data = document["data"];
        if (!data.HasMember("durl")) {
            return false;
        }

        rapidjson::Value& durl = data["durl"].GetArray();
        for (rapidjson::SizeType i = 0; i < durl.Size(); i++) {
            rapidjson::Value& value = durl[i];
            std::unique_ptr<Section> s = std::make_unique<Section>();
            if (!value.HasMember("order")) {
                return false;
            }
            if (!value.HasMember("url")) {
                return false;
            }
            s->order = value["order"].GetInt();
            s->section_url = value["url"].GetString();
            // 更新downloadinfo信息
            download_info_->durl.emplace_back(std::move(s));

            return true;
        }
    }

    return false;
};

bool UgcVideoDownloadTask::start_download(absl::string_view save_directory) {
    if (get_video_quality()) {
        if (parse_play_info()) {
            // 解析视频地址，视频后缀等
            fs::path path(save_directory);
            if (!fs::exists(path)) {
                if (std::filesystem::create_directory(save_directory.data())) {
                    return true;
                }
            } else {
                return true;
            }
        }
    };
    return false;
}

bool UgcVideoDownloadTask::download_video() {
    for (std::list<std::unique_ptr<Section>>::iterator it = download_info_->durl.begin();
         it != download_info_->durl.end();
         it++) {
        int order = (*it)->order;
        std::string url = (*it)->section_url;
        spdlog::info("{}\n", url.c_str());
        // 构建请求
        cpr::Url section_url = cpr::Url(url);
        cpr::Header header = cpr::Header{{"Referer", "https://www.bilibili.com"},
                                         {"User-Agent",
                                          "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
                                          "like Gecko) Chrome/90.0.4430.85 Safari/537.36 Edg/90.0.818.49"}};
        // 先发送head请求，获取数据content-length
        auto session = std::make_shared<cpr::Session>();

        session->SetUrl(section_url);
        session->SetHeader(header);
        cpr::Response header_response = session->Head();
        // 获取视频总长度
        int batch_size = 1024 * 1024 * 10;
        int content_length = std::stoi(header_response.header["Content-Length"]);
        // 计算视频的分段的数量
        int count = (content_length / batch_size) + ((content_length % batch_size) ? 1 : 0);
        spdlog::info("video content count {0}", count);

        if (count == 1) {
            char video_name[128];
            sprintf(video_name, "%s.%s", download_info_->video_name.c_str(), download_info_->video_format.c_str());
            std::ofstream of(video_name, std::ios::binary);
            session->SetRange(cpr::Range(0, content_length));
            session->Download(of);
            // 视频数量小视频直接下载
        } else {
#pragma omp parallel for num_threads(4)
            for (int i = 0; i < count; i++) {
                spdlog::info("OpenMP thread id : {}\n", omp_get_thread_num());
                char name[128];
                sprintf(name, "tmp_video_%d.%s", i, download_info_->video_format.c_str());
                std::ofstream of(name, std::ios::binary);
                if (i == count - 1) {
                    cpr::Range range = cpr::Range{i * batch_size, -1};
                    session->SetRange(range);
                } else {
                    cpr::Range range = cpr::Range{i * batch_size, (i + 1) * batch_size};
                    session->SetRange(range);
                };
                session->Download(of);
                // session->SetProgressCallback(cpr::ProgressCallback([&](cpr::cpr_off_t downloadTotal,
                //                                                        cpr::cpr_off_t downloadNow,
                //                                                        cpr::cpr_off_t uploadTotal,
                //                                                        cpr::cpr_off_t uploadNow,
                //                                                        intptr_t userdata) -> bool {
                //     std::cout << "Downloaded " << downloadNow << " / " << downloadTotal << " bytes." << std::endl;
                // }));
            }
        }
    }
    return true;
};

bool UgcVideoDownloadTask::end_download() {
    return true;
};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider