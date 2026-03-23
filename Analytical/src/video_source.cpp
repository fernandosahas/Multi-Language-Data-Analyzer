#include "video_source.h"
#include <iostream>

VideoSource::VideoSource(const std::string& uri) : uri_(uri) {}

bool VideoSource::open() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (cap_.isOpened()) return true;
    if (!cap_.open(uri_)) {
        std::cerr << "[VideoSource] Failed to open: " << uri_ << std::endl;
        return false;
    }
    return true;
}

bool VideoSource::read(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!cap_.isOpened()) return false;
    return cap_.read(frame);
}

double VideoSource::fps() const {
    return cap_.get(cv::CAP_PROP_FPS);
}

int VideoSource::width() const {
    return static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
}

int VideoSource::height() const {
    return static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
}

bool VideoSource::is_open() const {
    return cap_.isOpened();
}
