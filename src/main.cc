#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "cpr/cpr.h"
#include "spdlog/spdlog.h"
#include "spider_task.h"
ABSL_FLAG(std::string, url, "", "video url");

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    std::string url = absl::GetFlag(FLAGS_url);
    const absl::string_view dir = absl::string_view("./");

    spider::SpiderTask* task = new spider::SpiderTask(url, dir);
    task->run();
    delete task;
}