from api.core.rules import RuleEngine
from api.core.pipelines import PipelineManager
from api import alerts

_rule_engine = RuleEngine(alert_fn=lambda rule, payload: alerts.send_alert(
    rule.action.target, {"rule": rule.name, **payload}
))
_pipeline_manager = PipelineManager(_rule_engine)


def get_rule_engine():
    return _rule_engine


def get_pipeline_manager():
    return _pipeline_manager
