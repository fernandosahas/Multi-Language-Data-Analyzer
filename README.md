# Multi-Language-Data-Analyzer

Frontend (JS + D3.js)
        ↓
Flask Backend (Python API)
        ↓
----------------------------
| Python Engine (Pandas)   |
| R Engine (Statistics)    |
----------------------------
        ↓
Results → JSON → Frontend Charts



🧩 Folder Structure (Clean & Professional)
data-analyzer/
│
├── backend/
│   ├── app.py
│   ├── routes/
│   ├── services/
│   │   ├── python_analysis.py
│   │   ├── r_analysis.R
│   ├── uploads/
│
├── frontend/
│   ├── index.html
│   ├── app.js
│   ├── charts.js
│
├── models/
│   ├── predictor.pkl
│
├── README.md
