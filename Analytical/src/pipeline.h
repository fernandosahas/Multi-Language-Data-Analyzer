#pragma once

#include "video_source.h"
#include "preprocessor.h"
#include "inference_engine.h"
#include "postprocessor.h"
#include <atomic>
#include <thread>
#include <functional>

class Pipeline {
public:
    using DetectionCallback = std::function<void(const std::vector<Detection>&)>;

    Pipeline(const std::string& uri, const std::string& model_path);
    ~Pipeline();

    bool start(DetectionCallback cb = nullptr);
    void stop();
    bool running() const { return running_; }

private:
    void loop();

    VideoSource source_;
    Preprocessor pre_;
    InferenceEngine infer_;
    Postprocessor post_;

    std::atomic<bool> running_{false};
    std::thread worker_;
    DetectionCallback callback_;
};
