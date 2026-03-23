from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from pathlib import Path

from api.routes.health import router as health_router
from api.routes.pipelines import router as pipelines_router
from api.routes.rules import router as rules_router

app = FastAPI(title="AI Video Analytics Gateway", version="0.2.0")

app.include_router(health_router)
app.include_router(pipelines_router)
app.include_router(rules_router)

static_dir = Path(__file__).parent / "static"
if static_dir.exists():
    app.mount("/ui", StaticFiles(directory=static_dir, html=True), name="ui")
