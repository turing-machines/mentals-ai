#pragma once

#include "core.h"

class ProgressBar {
public:
    ProgressBar(
        const std::string& prefix = "",
        const std::string& suffix = "",
        int length = 50,
        char fill = '#')
        : __prefix(prefix), __suffix(suffix), __length(length), __fill(fill), __current_progress(0) {}

    void update(float percent) {
        __current_progress = percent;
        print_progress_bar(percent);
    }

private:
    std::string __prefix;
    std::string __suffix;
    int __length;
    char __fill;
    float __current_progress;

    void print_progress_bar(float percent) {
        int filled_length = static_cast<int>(__length * percent / 100.0);

        std::cout << "\r" << __prefix << " [";
        for (int i = 0; i < __length; ++i) {
            if (i < filled_length) {
                std::cout << __fill;
            } else {
                std::cout << ' ';
            }
        }
        std::cout << "] " << int(percent) << "% " << __suffix << std::flush;
    }
};
