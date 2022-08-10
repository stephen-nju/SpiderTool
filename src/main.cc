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
    absl::string_view url = "https://www.bilibili.com/video/BV16A411v7AE?from=search&seid=5255426327508319378     ";
    const absl::string_view dir = absl::string_view("./");

    spider::SpiderTask* task = new spider::SpiderTask(url, dir);
    task->run();
    delete task;
}