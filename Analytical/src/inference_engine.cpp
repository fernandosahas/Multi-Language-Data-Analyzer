#include "inference_engine.h"
#include <iostream>
#include <algorithm>

namespace {
// YOLOv5/v8 style output parsing: rows of [x,y,w,h,conf,cls...]
std::vector<std::string> default_classes() {
    return {"person"};
}
}  // namespace

InferenceEngine::InferenceEngine(const std::string& model_path)
    : model_path_(model_path) {
    hog_.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

bool InferenceEngine::load() {
    try {
        if (!model_path_.empty()) {
            net_ = cv::dnn::readNet(model_path_);
            if (net_.empty()) {
                std::cerr << "[InferenceEngine] Failed to load model: " << model_path_
                          << " falling back to HOG.\n";
            } else {
                net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
                net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
                use_yolo_ = true;
                std::cerr << "[InferenceEngine] YOLO/ONNX loaded: " << model_path_ << "\n";
            }
        } else {
            std::cerr << "[InferenceEngine] Model path empty; using HOG fallback.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[InferenceEngine] Exception loading model: " << e.what()
                  << " using HOG.\n";
    }
    ready_ = true;
    return ready_;
}

std::vector<Detection> InferenceEngine::infer(const cv::Mat& letterboxed, const PreprocessMeta& meta) {
    if (!ready_) return {};
    if (use_yolo_) {
        return infer_yolo(letterboxed, meta);
    }
    return infer_hog(letterboxed, meta);
}

std::vector<Detection> InferenceEngine::infer_hog(const cv::Mat& frame, const PreprocessMeta& meta) {
    std::vector<Detection> detections;
    std::vector<cv::Rect> found;
    std::vector<double> weights;
    hog_.detectMultiScale(frame, found, weights);

    for (size_t i = 0; i < found.size(); ++i) {
        // Map back to original size
        int x = static_cast<int>((found[i].x - meta.pad_x) / meta.scale);
        int y = static_cast<int>((found[i].y - meta.pad_y) / meta.scale);
        int w = static_cast<int>(found[i].width / meta.scale);
        int h = static_cast<int>(found[i].height / meta.scale);
        cv::Rect box(x, y, w, h);
        detections.push_back(Detection{box, static_cast<float>(weights[i]), "person", 0});
    }
    return detections;
}

std::vector<Detection> InferenceEngine::infer_yolo(const cv::Mat& letterboxed, const PreprocessMeta& meta) {
    std::vector<Detection> out;
    if (net_.empty()) return out;

    cv::Mat blob = cv::dnn::blobFromImage(
        letterboxed, 1.0 / 255.0, meta.target_size, cv::Scalar(), true, false);
    net_.setInput(blob);
    std::vector<cv::Mat> outputs;
    net_.forward(outputs, net_.getUnconnectedOutLayersNames());

    const auto& classes = default_classes(); // ALERT: load class names from file if available.
    float conf_thresh = 0.4f;
    float nms_thresh = 0.45f;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (const auto& output : outputs) {
        const float* data = reinterpret_cast<const float*>(output.data);
        for (int i = 0; i < output.rows; ++i, data += output.cols) {
            float obj_conf = data[4];
            if (obj_conf < conf_thresh) continue;
            // find best class
            cv::Mat scores(1, output.cols - 5, CV_32FC1, (void*)(data + 5));
            cv::Point classIdPoint;
            double maxClassScore;
            cv::minMaxLoc(scores, 0, &maxClassScore, 0, &classIdPoint);
            float conf = obj_conf * static_cast<float>(maxClassScore);
            if (conf < conf_thresh) continue;

            float cx = data[0];
            float cy = data[1];
            float w = data[2];
            float h = data[3];

            // YOLO outputs are relative to letterboxed size; map back to original.
            float x0 = (cx - w * 0.5f - meta.pad_x) / meta.scale;
            float y0 = (cy - h * 0.5f - meta.pad_y) / meta.scale;
            float x1 = (cx + w * 0.5f - meta.pad_x) / meta.scale;
            float y1 = (cy + h * 0.5f - meta.pad_y) / meta.scale;

            int left = std::max(0, static_cast<int>(std::round(x0)));
            int top = std::max(0, static_cast<int>(std::round(y0)));
            int width = std::min(meta.original_size.width - left,
                                 static_cast<int>(std::round(x1 - x0)));
            int height = std::min(meta.original_size.height - top,
                                  static_cast<int>(std::round(y1 - y0)));

            boxes.emplace_back(left, top, width, height);
            confidences.push_back(conf);
            class_ids.push_back(classIdPoint.x);
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_thresh, nms_thresh, indices);

    for (int idx : indices) {
        int cls_id = class_ids[idx];
        std::string label = (cls_id >= 0 && cls_id < static_cast<int>(classes.size()))
                                ? classes[cls_id]
                                : "cls_" + std::to_string(cls_id);
        out.push_back(Detection{boxes[idx], confidences[idx], label, cls_id});
    }
    return out;
}
