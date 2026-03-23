#include "postprocessor.h"

cv::Mat Postprocessor::draw(const cv::Mat& frame, const std::vector<Detection>& detections) const {
    cv::Mat out = frame.clone();
    for (const auto& det : detections) {
        cv::rectangle(out, det.box, cv::Scalar(0, 255, 0), 2);
        std::string text = det.label + " " + std::to_string(det.score);
        cv::putText(out, text, det.box.tl() + cv::Point(0, -5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }
    return out;
}
