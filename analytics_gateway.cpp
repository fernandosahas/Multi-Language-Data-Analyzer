#include "pipeline.h"
#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> keep_running{true};

void on_signal(int) {
    keep_running = false;
}

int main(int argc, char** argv) {
    std::string uri = "0"; // default camera
    std::string model_path;
    int max_frames = 200;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--source" && i + 1 < argc) uri = argv[++i];
        else if (arg == "--model" && i + 1 < argc) model_path = argv[++i];
        else if (arg == "--max-frames" && i + 1 < argc) max_frames = std::stoi(argv[++i]);
    }

    std::signal(SIGINT, on_signal);

    Pipeline pipeline(uri, model_path);
    int frames_seen = 0;
    auto cb = [&](const std::vector<Detection>& detections) {
        ++frames_seen;
        std::cout << "Frame " << frames_seen << " detections: " << detections.size() << std::endl;
        if (frames_seen >= max_frames) keep_running = false;
    };

    if (!pipeline.start(cb)) {
        std::cerr << "Failed to start pipeline\n";
        return 1;
    }

    while (keep_running && pipeline.running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    pipeline.stop();
    std::cout << "Stopped after " << frames_seen << " frames\n";
    return 0;
}
