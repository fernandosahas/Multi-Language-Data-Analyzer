#include "pipeline.h"
#include <iostream>
#include <chrono>

Pipeline::Pipeline(const std::string& uri, const std::string& model_path)
    : source_(uri), infer_(model_path) {}

Pipeline::~Pipeline() { stop(); }

bool Pipeline::start(DetectionCallback cb) {
    if (running_) return true;
    callback_ = cb;

    if (!source_.open()) {
        std::cerr << "[Pipeline] Unable to open source\n";
        return false;
    }
    if (!infer_.load()) {
        std::cerr << "[Pipeline] Inference failed to load\n";
        return false;
    }

    running_ = true;
    worker_ = std::thread(&Pipeline::loop, this);
    return true;
}

void Pipeline::stop() {
    if (!running_) return;
    running_ = false;
    if (worker_.joinable()) worker_.join();
}

void Pipeline::loop() {
    cv::Mat frame;
    while (running_) {
        if (!source_.read(frame)) {
            std::cerr << "[Pipeline] Frame read failed\n";
            break;
        }
        PreprocessMeta meta;
        cv::Mat processed;
        pre_.process(frame, processed, meta);
        auto detections = infer_.infer(processed, meta);
        if (callback_) {
            callback_(detections);
        }
        // simulate small pacing to avoid tight loop
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    running_ = false;
}
