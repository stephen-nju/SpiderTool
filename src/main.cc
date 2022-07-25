#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "cpr/cpr.h"
#include "utils.h"

int main(int argc, char** argv) {
    const string s = string("  hello word ");
    string d = spider::trim(s);
    printf("%s", s.c_str());
}