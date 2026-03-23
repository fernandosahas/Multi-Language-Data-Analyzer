#include "preprocessor.h"
#include <cmath>

Preprocessor::Preprocessor(int target_width, int target_height)
    : target_width_(target_width), target_height_(target_height) {}

// Letterbox to target size while keeping aspect; returns scale + padding.
void Preprocessor::process(const cv::Mat& frame, cv::Mat& out, PreprocessMeta& meta) const {
    meta.original_size = frame.size();
    meta.target_size = cv::Size(target_width_, target_height_);

    const float r = std::min(
        static_cast<float>(target_width_) / frame.cols,
        static_cast<float>(target_height_) / frame.rows);
    int new_w = static_cast<int>(std::round(frame.cols * r));
    int new_h = static_cast<int>(std::round(frame.rows * r));

    cv::Mat resized;
    cv::resize(frame, resized, cv::Size(new_w, new_h));

    meta.pad_x = (target_width_ - new_w) / 2;
    meta.pad_y = (target_height_ - new_h) / 2;
    meta.scale = r;

    out = cv::Mat::zeros(target_height_, target_width_, frame.type());
    resized.copyTo(out(cv::Rect(meta.pad_x, meta.pad_y, resized.cols, resized.rows)));
}
