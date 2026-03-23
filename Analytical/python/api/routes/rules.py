from fastapi import APIRouter, HTTPException, Depends
from pydantic import BaseModel
import uuid

from api.state import get_rule_engine
from api.core.rules import Rule, RuleCondition, RuleAction

router = APIRouter(prefix="/rules", tags=["rules"])


class RulePayload(BaseModel):
    name: str
    label: str
    min_score: float = 0.5
    min_count: int = 1
    within_seconds: float = 2.0
    action_kind: str = "webhook"
    action_target: str


@router.get("")
def list_rules(engine=Depends(get_rule_engine)):
    return engine.list()


@router.post("")
def create_rule(payload: RulePayload, engine=Depends(get_rule_engine)):
    rule_id = uuid.uuid4().hex
    rule = Rule(
        id=rule_id,
        name=payload.name,
        condition=RuleCondition(
            label=payload.label,
            min_score=payload.min_score,
            min_count=payload.min_count,
            within_seconds=payload.within_seconds,
        ),
        action=RuleAction(kind=payload.action_kind, target=payload.action_target),
    )
    engine.add(rule)
    return {"id": rule_id, "message": "created"}


@router.delete("/{rule_id}")
def delete_rule(rule_id: str, engine=Depends(get_rule_engine)):
    if not engine.remove(rule_id):
        raise HTTPException(status_code=404, detail="rule not found")
    return {"id": rule_id, "message": "deleted"}
