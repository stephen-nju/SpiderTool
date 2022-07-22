#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "cpr/cpr.h"
int main(int argc, char **argv)
{
    cpr::Response r = cpr::Get(cpr::Url{"https://www.bilibili.com/?spm_id_from=333.337.0.0"});

    // std::cout << "Status code: " << r.status_code << '\n';
    // std::cout << "Header:\n";
    // for (const std::pair<const std::basic_string<char>, std::basic_string<char>> &kv : r.header)
    // {
    //     std::cout << '\t' << kv.first << ':' << kv.second << '\n';
    // }
    // std::cout << "Text: " << r.text << '\n';
    printf("%s", r.text.c_str());
    return 0;
}