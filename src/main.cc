#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "cpr/cpr.h"
#include "spdlog/spdlog.h"
#include "spider_task.h"
int main(int argc, char** argv) {
    spdlog::info("start spider......\n");
    absl::string_view url = "https://www.bilibili.com/video/BV1UT411L71N?spm_id_from=333.1007.tianma.2-3-6.click";
    const absl::string_view dir = absl::string_view("./");

    spider::SpiderTask* task = new spider::SpiderTask(url, dir);
    task->run();
    delete task;
}