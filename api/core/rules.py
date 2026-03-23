from dataclasses import dataclass, field
from typing import Callable, Dict, List, Optional
import time

try:
    import analytics  # detection struct
except ImportError:
    analytics = None  # type: ignore


@dataclass
class RuleCondition:
    label: str
    min_score: float = 0.5
    min_count: int = 1
    within_seconds: float = 2.0


@dataclass
class RuleAction:
    kind: str  # "webhook" / "log"
    target: str


@dataclass
class Rule:
    id: str
    name: str
    condition: RuleCondition
    action: RuleAction
    created_at: float = field(default_factory=time.time)


class RuleEngine:
    def __init__(self, alert_fn: Optional[Callable[[Rule, Dict], None]] = None):
        self.rules: Dict[str, Rule] = {}
        self.alert_fn = alert_fn

    def add(self, rule: Rule):
        self.rules[rule.id] = rule

    def remove(self, rule_id: str) -> bool:
        return self.rules.pop(rule_id, None) is not None

    def list(self):
        return list(self.rules.values())

    def process_event(self, pipeline_id: str, detections: List["analytics.Detection"]):
        now = time.time()
        for rule in self.rules.values():
            count = sum(
                1 for d in detections
                if d.label == rule.condition.label and d.score >= rule.condition.min_score
            )
            if count >= rule.condition.min_count:
                payload = {
                    "rule": rule.name,
                    "pipeline_id": pipeline_id,
                    "count": count,
                    "timestamp": now,
                }
                if self.alert_fn:
                    self.alert_fn(rule, payload)
