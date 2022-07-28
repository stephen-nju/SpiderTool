#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "cpr/cpr.h"
#include "libnlptool/utils.h"

int main(int argc, char** argv) {
    std::string url = "  www.bilibili.com ";
    absl::string_view o = nlptools::lstrip(absl::string_view(url));
    printf("%s\n", o.data());
}