import httpx

def send_alert(webhook: str, payload: dict) -> None:
    # ALERT: Harden with auth/retries and map to your alerting provider.
    try:
        httpx.post(webhook, json=payload, timeout=2.0)
    except Exception as exc:
        print(f"[alerts] failed to send alert: {exc}")
