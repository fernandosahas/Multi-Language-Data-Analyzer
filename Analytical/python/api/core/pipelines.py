import threading
import uuid
import time
from typing import Dict, Optional, Callable, List

from api.core.rules import RuleEngine

try:
    import analytics  # built by pybind11
except ImportError as exc:
    raise RuntimeError("analytics module not found. Build C++ extension first.") from exc


class PipelineHandle:
    def __init__(self, source: str, model_path: str = ""):
        self.source = source
        self.model_path = model_path
        self.pipeline = analytics.Pipeline(source, model_path)
        self.thread: Optional[threading.Thread] = None
        self.running = False
        self.frame_count = 0
        self.detection_count = 0
        self.last_started = None

    def start(self, on_detections: Optional[Callable[[List[analytics.Detection]], None]] = None):
        if self.running:
            return
        self.running = True
        self.last_started = time.time()

        def loop():
            if not self.pipeline.start(on_detections):
                self.running = False
                return
            while self.running and self.pipeline.running():
                time.sleep(0.02)

        self.thread = threading.Thread(target=loop, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False
        self.pipeline.stop()
        if self.thread and self.thread.is_alive():
            self.thread.join(timeout=1)


class PipelineManager:
    def __init__(self, rules: RuleEngine):
        self._pipelines: Dict[str, PipelineHandle] = {}
        self._rules = rules

    def start(self, source: str, model_path: str = "") -> str:
        pid = uuid.uuid4().hex
        handle = PipelineHandle(source, model_path)

        def on_detections(detections):
            handle.frame_count += 1
            handle.detection_count += len(detections)
            self._rules.process_event(pid, detections)

        handle.start(on_detections)
        self._pipelines[pid] = handle
        return pid

    def stop(self, pid: str) -> bool:
        handle = self._pipelines.get(pid)
        if not handle:
            return False
        handle.stop()
        del self._pipelines[pid]
        return True

    def list(self):
        return {
            pid: {
                "source": h.source,
                "model_path": h.model_path,
                "frames": h.frame_count,
                "detections": h.detection_count,
                "running": h.running,
                "started_at": h.last_started,
            }
            for pid, h in self._pipelines.items()
        }
