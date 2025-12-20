#pragma once
#include <string>
#include <debugapi.h>

static void DebugLog(const std::string& msg) {
    std::string line = msg + "\r\n";
    OutputDebugStringA(msg.c_str());
}
