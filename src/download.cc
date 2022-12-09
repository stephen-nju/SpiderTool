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
#define BUFFER_SIZE 1024

bool write_data(std::string data, intptr_t userdata) {
    FILE* pf = reinterpret_cast<FILE*>(userdata);
    if (pf != nullptr) {
        // std::cout << "userdata=" << userdata << "  recv data size: " << data.size() << std::endl;
        fwrite(data.data(), sizeof(char), data.size(), pf);
        return true;
    }
    return false;
};

DownloadTask::DownloadTask(){};

bool DownloadTask::run(absl::string_view save_directory) {
    if (!start_download()) {
        return false;
    };
    if (!download_video(save_directory)) {
        return false;
    };
    if (!end_download()) {
        return false;
    }
    return true;
};

DownloadTask::DownloadInfo::DownloadInfo(){};

DownloadTask::DownloadInfo::~DownloadInfo(){};

DownloadTask::~DownloadTask() {
    spdlog::info("runing DownloadTask Deconstruction");
};

UgcVideoDownloadTask::UgcVideoDownloadTask(std::unique_ptr<VideoInfo> video_info) : DownloadTask() {
    video_info_ = std::move(video_info);
    // 初始化downloadinfo，后面解析数据
    download_info_ = std::make_unique<UgcDownloadInfo>();
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
    std::string title = *video_info_->title;
    return title;
};

bool UgcVideoDownloadTask::get_video_quality() {
    cpr::Header header = cpr::Header{
        {"User-Agent", "Mozilla/5.0"},
        {"Referer", "https://www.bilibili.com"},
    };
    cpr::Session session;
    char raw_api[512];
    int n = sprintf(raw_api,
                    "https://api.bilibili.com/x/player/playurl?avid=%d&cid=%d&qn=64&fourk=1",
                    video_info_->aid,
                    video_info_->cid);
    // 初始化下载质量为64，720p高清
    if (n > 0) {
        cpr::Url url = cpr::Url(raw_api);
        session.SetUrl(url);
        session.SetHeader(header);

        cpr::Response response = session.Get();
        // spdlog::info(raw_api);

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
        std::vector<int> video_accept_quality;
        for (rapidjson::SizeType i = 0; i < accept_quality.Size(); i++) {
            rapidjson::Value& value = accept_quality[i];
            video_accept_quality.push_back(value.GetInt());
        };
        for (auto q = 0; q < video_accept_quality.size(); q++) {
            spdlog::info("video resolution ratio :{0}, code:{1}\n",
                         quality_map[video_accept_quality.at(q)],
                         video_accept_quality.at(q));
        }
        // spdlog::info("choose your video downlaod quality\n");
        // 优先选择最高分辨率
        // scanf("%d", &video_download_quality_);
        download_info_->video_quality = video_accept_quality.at(1);

        download_info_->video_format = quality_format_map->at(download_info_->video_quality);
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
                    download_info_->video_quality);
    // spdlog::info(api);

    if (n > 0) {
        cpr::Response response = cpr::Get(cpr::Url(api));
        rapidjson::Document document;
        // spdlog::info(response.text.c_str());
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
        if (durl.Size() == 0) {
            spdlog::error("error request,durl is none");
            return false;
        } else if (durl.Size() > 1) {
            spdlog::error("该视频当前画质有分段，不支持下载");
            return false;
        }
        rapidjson::Value& value = durl[0];
        if (!value.HasMember("order")) {
            return false;
        }
        if (!value.HasMember("url")) {
            return false;
        }
        download_info_->durl = value["url"].GetString();

        // s->order = value["order"].GetInt();
        // s->section_url = value["url"].GetString();
        // 更新downloadinfo信息

        return true;
    }
    return false;
};

bool UgcVideoDownloadTask::start_download() {
    if (!get_video_quality()) {
        return false;
    }
    if (!parse_play_info()) {
        return false;
    }
    return true;
}

bool UgcVideoDownloadTask::download_video(absl::string_view save_directory) {
    fs::path directory(save_directory);
    if (!fs::exists(directory)) {
        if (!std::filesystem::create_directory(directory)) {
            return false;
        }
    }
    std::string url = download_info_->durl;
    // spdlog::info("{}\n", url.c_str());
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
    char video_name[128];
    sprintf(video_name, "%s.%s", this->get_title().c_str(), download_info_->video_format.c_str());
    // 复制字符串
    sprintf(video_path_, "%s", video_name);
    if (count == 1) {
        std::ofstream of(video_name, std::ios::binary);
        session->SetRange(cpr::Range(0, content_length));
        session->Download(of);
        download_info_->merge_finished = true;
        // 视频数量小视频直接下载
    } else {
        // 创建临时目录
        char temp[128];
        sprintf(temp, "%s%s", save_directory.data(), this->get_title().c_str());
        // windows 需要系统语言utf8支持
        fs::path tmp_dir = temp;
        if (!fs::exists(tmp_dir)) {
            if (!std::filesystem::create_directory(tmp_dir)) {
                return false;
            }
        }
#pragma omp parallel for num_threads(4)
        for (int i = 0; i < count; i++) {
            auto section_sess = std::make_shared<cpr::Session>();
            section_sess->SetUrl(section_url);
            section_sess->SetHeader(header);
            spdlog::info("OpenMP thread id : {}\n", omp_get_thread_num());
            char name[128];
            sprintf(name, "tmp_video_%d.%s", i, download_info_->video_format.c_str());
            fs::path video_path(tmp_dir);
            video_path.append(name);
            // spdlog::info("{}", video_path.string());
            download_info_->temp_files.emplace_back(video_path.string());
            FILE* output_file = fopen(video_path.string().c_str(), "wb");
            // std::ofstream of(name, std::ios::binary);
            if (i == count - 1) {
                cpr::Range range = cpr::Range{i * batch_size, -1};
                section_sess->SetRange(range);
            } else {
                cpr::Range range = cpr::Range{i * batch_size, (i + 1) * batch_size};
                section_sess->SetRange(range);
            };
            // section_sess->SetProgressCallback(cpr::ProgressCallback([&](cpr::cpr_off_t downloadTotal,
            //                                                             cpr::cpr_off_t downloadNow,
            //                                                             cpr::cpr_off_t uploadTotal,
            //                                                             cpr::cpr_off_t uploadNow,
            //                                                             intptr_t userdata) -> bool {
            //     std::cout << "Downloaded " << downloadNow << " / " << downloadTotal << " bytes." << std::endl;
            //     return true;
            // }));
            // section_sess->Download(of);
            section_sess->Download(cpr::WriteCallback{write_data, reinterpret_cast<intptr_t>(output_file)});
            fclose(output_file);
        }

        download_info_->dowload_finished = true;
    }

    return true;
};

bool UgcVideoDownloadTask::end_download() {
    // 合并下载的文件
    if (!download_info_->dowload_finished) {
        return false;
    }
    if (!download_info_->merge_finished) {
        FILE* output = fopen(video_path_, "wb+");
        if (output = nullptr) {
            spdlog::error("merge files error can not open file");
            fclose(output);
            return false;
        }
        for (auto p = download_info_->temp_files.begin(); p != download_info_->temp_files.end(); p++) {
            std::string video_path = *p;
            FILE* input = fopen(video_path.c_str(), "rb");
            if (input == nullptr) {
                spdlog::error("can not open input file");
                fclose(input);
                fclose(output);
                return false;
            }
            unsigned char buf[BUFFER_SIZE];
            while (!feof(input)) {
                size_t n = fread(buf, sizeof(unsigned char), BUFFER_SIZE, input);
                fwrite(buf, sizeof(unsigned char), n, output);
            };
            fclose(input);
        }
        fclose(output);
    }
    return true;
};
UgcVideoDownloadTask::~UgcVideoDownloadTask(){};

}  // namespace spider