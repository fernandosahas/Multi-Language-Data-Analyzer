#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <mutex>

class VideoSource {
public:
    explicit VideoSource(const std::string& uri);
    bool open();
    bool read(cv::Mat& frame);
    double fps() const;
    int width() const;
    int height() const;
    bool is_open() const;

private:
    std::string uri_;
    cv::VideoCapture cap_;
    std::mutex mtx_;
};
