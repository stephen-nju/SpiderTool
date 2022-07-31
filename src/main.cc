#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "cpr/cpr.h"
#include"spider_task.h"
#include <memory>
int main(int argc, char** argv) {
    
printf("start spider......");
cpr::Url url=cpr::Url("https://www.bilibili.com/video/BV17Z4y1F7VN?from=search&seid=2171182556647259628&spm_id_from=333.337.0.0");
const std::string dir=std::string("/Users/zhubin/Documents");

spider::SpiderTask task(url,dir);



}