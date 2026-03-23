#pragma once

#include "inference_engine.h"
#include <opencv2/opencv.hpp>
#include <vector>

class Postprocessor {
public:
    cv::Mat draw(const cv::Mat& frame, const std::vector<Detection>& detections) const;
};
