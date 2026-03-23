# AI-Powered Real-Time Video Analytics Gateway

High-performance C++ core (OpenCV + lightweight inference) with a Python FastAPI management layer exposed through `pybind11`. The C++ side ingests RTSP/USB streams, pre-processes frames, runs placeholder inference, and returns detections. The Python side offers REST endpoints for configuration, health, and alerting hooks.

> ALERT: Replace the placeholder inference in `src/inference_engine.cpp` with your real model (ONNX/TensorRT/YOLO, etc.).
> ALERT: Update `config/config.example.yaml` with your camera URLs, model paths, and alert webhooks.

## Layout
- `CMakeLists.txt` – builds the C++ library, sample binary, and Python extension.
- `src/` – C++ pipeline (video I/O, preprocessing, YOLO/ONNX or HOG fallback).
- `bindings/py_module.cpp` – `pybind11` bridge exposing the pipeline to Python.
- `python/api/` – FastAPI server with routers (`/pipelines`, `/rules`, `/health`) plus a lightweight UI under `/ui`.
- `config/config.example.yaml` – editable runtime config.

## Build (C++ + Python)

Prereqs: CMake ≥3.20, C++17 compiler, OpenCV ≥4.5, pybind11, Python ≥3.10. On Windows, vcpkg is easiest.

```bash
# Windows (PowerShell, vcpkg assumed at C:\vcpkg)
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_PYTHON=ON
cmake --build build --config Release

# Linux/macOS
cmake -S . -B build -DBUILD_PYTHON=ON
cmake --build build -j
```

The Python extension is built into `build/python/analytics` (editable install):
```bash
pip install -r python/requirements.txt
python -m pip install -e python
# Make CMake-built extension visible
set PYTHONPATH=%CD%/build/python;%PYTHONPATH%   # Windows
export PYTHONPATH=$PWD/build/python:$PYTHONPATH # Linux/macOS
```

## Run
```bash
# Sample C++ binary (prints detections per frame)
./build/gateway_app --source rtsp://... --max-frames 50

# FastAPI service
uvicorn api.main:app --reload --port 8000 --app-dir python
# UI
# Visit http://localhost:8000/ui for the control panel.
```

## API (selected)
- `POST /pipelines` – start a pipeline for a camera.
- `DELETE /pipelines/{id}` – stop it.
- `GET /pipelines` – list active pipelines.
- `GET /health` – liveness probe.
- `POST /rules` / `GET /rules` / `DELETE /rules/{id}` – manage rule engine that triggers webhooks on detection matches.

## Next Steps
- Swap in real models and GPU backends.
- Wire alerts to Slack/Email/HTTP in `python/api/alerts.py` (placeholder) and load rule definitions from `config/config.yaml`.
- Add persistence (PostgreSQL) and metrics (Prometheus).
