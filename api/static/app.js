const api = (path, opts = {}) =>
  fetch(path, { headers: { "Content-Type": "application/json" }, ...opts }).then((r) =>
    r.ok ? r.json() : r.text().then((t) => Promise.reject(t))
  );

const renderPipelines = async () => {
  const tbody = document.querySelector("#pipelines-table tbody");
  tbody.innerHTML = "";
  const items = await api("/pipelines");
  Object.entries(items).forEach(([id, p]) => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><code>${id.slice(0, 8)}</code></td>
      <td>${p.source}</td>
      <td>${p.frames}</td>
      <td>${p.detections}</td>
      <td>${p.running ? "running" : "stopped"}</td>
      <td><button data-id="${id}" class="ghost">Stop</button></td>`;
    tbody.appendChild(tr);
  });
};

const renderRules = async () => {
  const list = document.querySelector("#rules-list");
  list.innerHTML = "";
  const rules = await api("/rules");
  rules.forEach((r) => {
    const li = document.createElement("li");
    li.innerHTML = `<div><strong>${r.name}</strong><div class="pill">${r.condition.label} ≥ ${r.condition.min_count} @ ${r.condition.min_score}</div></div><button class="ghost" data-rule="${r.id}">Delete</button>`;
    list.appendChild(li);
  });
};

document.getElementById("start-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const source = document.getElementById("source").value;
  const model = document.getElementById("model").value;
  await api("/pipelines", {
    method: "POST",
    body: JSON.stringify({ source, model_path: model }),
  });
  await renderPipelines();
});

document.querySelector("#pipelines-table tbody").addEventListener("click", async (e) => {
  if (e.target.dataset.id) {
    await api(`/pipelines/${e.target.dataset.id}`, { method: "DELETE" });
    await renderPipelines();
  }
});

document.getElementById("rule-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const payload = {
    name: document.getElementById("rule-name").value,
    label: document.getElementById("rule-label").value,
    min_count: parseInt(document.getElementById("rule-count").value, 10),
    min_score: parseFloat(document.getElementById("rule-score").value),
    action_target: document.getElementById("rule-webhook").value,
  };
  await api("/rules", { method: "POST", body: JSON.stringify(payload) });
  await renderRules();
});

document.getElementById("rules-list").addEventListener("click", async (e) => {
  if (e.target.dataset.rule) {
    await api(`/rules/${e.target.dataset.rule}`, { method: "DELETE" });
    await renderRules();
  }
});

document.getElementById("refresh").addEventListener("click", () => {
  renderPipelines();
  renderRules();
});

renderPipelines();
renderRules();
