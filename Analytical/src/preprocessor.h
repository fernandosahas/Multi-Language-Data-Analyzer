#pragma once

#include <opencv2/opencv.hpp>

struct PreprocessMeta {
    float scale{1.0f};
    int pad_x{0};
    int pad_y{0};
    cv::Size original_size;
    cv::Size target_size;
};

class Preprocessor {
public:
    Preprocessor(int target_width = 640, int target_height = 640);
    void process(const cv::Mat& frame, cv::Mat& out, PreprocessMeta& meta) const;

private:
    int target_width_;
    int target_height_;
};
