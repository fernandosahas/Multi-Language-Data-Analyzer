from fastapi import APIRouter, HTTPException, Depends
from pydantic import BaseModel
from typing import Optional

from api.state import get_pipeline_manager

router = APIRouter(prefix="/pipelines", tags=["pipelines"])


class PipelineCreate(BaseModel):
    source: str
    model_path: Optional[str] = ""


@router.get("")
def list_pipelines(manager=Depends(get_pipeline_manager)):
    return manager.list()


@router.post("")
def start_pipeline(payload: PipelineCreate, manager=Depends(get_pipeline_manager)):
    pid = manager.start(payload.source, payload.model_path or "")
    return {"id": pid, "message": "started"}


@router.delete("/{pid}")
def stop_pipeline(pid: str, manager=Depends(get_pipeline_manager)):
    if not manager.stop(pid):
        raise HTTPException(status_code=404, detail="pipeline not found")
    return {"id": pid, "message": "stopped"}
