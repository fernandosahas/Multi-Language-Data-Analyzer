#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#include "preprocessor.h"

struct Detection {
    cv::Rect box;
    float score;
    std::string label;
    int class_id{0};
};

class InferenceEngine {
public:
    explicit InferenceEngine(const std::string& model_path);
    bool load();
    std::vector<Detection> infer(const cv::Mat& letterboxed, const PreprocessMeta& meta);

private:
    std::vector<Detection> infer_hog(const cv::Mat& frame, const PreprocessMeta& meta);
    std::vector<Detection> infer_yolo(const cv::Mat& letterboxed, const PreprocessMeta& meta);

    std::string model_path_;
    cv::HOGDescriptor hog_; // simple CPU baseline
    cv::dnn::Net net_;      // YOLO/ONNX
    bool use_yolo_{false};
    bool ready_{false};
};
