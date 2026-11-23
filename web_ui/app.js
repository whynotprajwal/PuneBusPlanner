const state = {
  network: null,
  lastPlan: null,
};

const LINE_COLORS = {
  PurpleLine: "#a78bfa",
  AquaLine: "#22d3ee",
  RedLine: "#fb7185",
  GreenLine: "#34d399",
};

const FALLBACK_LINE_COLORS = ["#38bdf8", "#fbbf24", "#f472b6", "#4ade80", "#c084fc"];

const $ = (id) => document.getElementById(id);

document.addEventListener("DOMContentLoaded", init);

async function init() {
  bindTabs();
  $("refreshNetwork").addEventListener("click", () => loadNetwork());
  $("planForm").addEventListener("submit", onPlan);
  $("compareForm").addEventListener("submit", onCompare);
  $("bookButton").addEventListener("click", onBook);
  await loadNetwork();
  activateFromHash();
}

function bindTabs() {
  const tabs = [...document.querySelectorAll(".tab")];
  tabs.forEach((tab) => {
    tab.addEventListener("click", () => {
      activatePanel(tab.dataset.panel);
      const slug = tab.dataset.panel.replace("Panel", "").toLowerCase();
      history.replaceState(null, "", `#${slug}`);
    });
  });
}

function activatePanel(panelId) {
  const tabs = [...document.querySelectorAll(".tab")];
  const panels = [...document.querySelectorAll(".panel")];
  tabs.forEach((tab) => tab.classList.toggle("active", tab.dataset.panel === panelId));
  panels.forEach((panel) => panel.classList.toggle("active", panel.id === panelId));
}

function activateFromHash() {
  const hash = location.hash.replace("#", "");
  if (hash === "plan") activatePanel("planPanel");
  if (hash === "compare") activatePanel("comparePanel");
}

async function api(path, body) {
  const options = body
    ? {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
      }
    : {};

  const response = await fetch(path, options);
  let data;
  try {
    data = await response.json();
  } catch {
    throw new Error(`Backend returned invalid JSON (HTTP ${response.status}).`);
  }
  if (!response.ok || !data.ok) {
    throw new Error(data.error || `Request failed (HTTP ${response.status}).`);
  }
  return data;
}

async function loadNetwork({ populate = true } = {}) {
  try {
    const data = await api("/api/network");
    state.network = data.network;
    renderNetwork(data.network);
    if (populate) populateSelectors(data.network);
    setStatus("Backend connected", "online");
  } catch (error) {
    setStatus("Backend offline", "offline");
    showToast(error.message, "error");
  }
}

function populateSelectors(network) {
  const stops = [...network.stops].sort((a, b) => a.id - b.id);
  const options = stops
    .map((stop) => `<option value="${stop.id}">${escapeHtml(displayName(stop.name))}</option>`)
    .join("");
  const ids = ["fromStop", "toStop", "compareFrom", "compareTo"];
  const previous = {};
  ids.forEach((id) => {
    previous[id] = $(id).value;
  });

  ids.forEach((id) => {
    const select = $(id);
    select.innerHTML = options;
    if (previous[id] && stops.some((stop) => String(stop.id) === previous[id])) {
      select.value = previous[id];
    }
  });

  const defaultTo = stops.find((stop) => stop.id === 19)?.id ?? stops.at(-1)?.id ?? "";
  if ($("fromStop").value === $("toStop").value) $("toStop").value = defaultTo;
  if ($("compareFrom").value === $("compareTo").value) $("compareTo").value = defaultTo;

  const busOptions = network.fleet
    .map((bus) => `<option value="${bus.index}">${bus.index} · ${escapeHtml(bus.type)}</option>`)
    .join("");
  const busSelect = $("bus");
  const previousBus = busSelect.value;
  busSelect.innerHTML = busOptions;
  if (previousBus) busSelect.value = previousBus;
  if (!busSelect.value && network.fleet.length > 1) busSelect.value = "2";
}

function renderNetwork(network) {
  const stats = [
    ["Stops", network.stops.length],
    ["Roads", network.roads.length],
    ["Lines", network.lines.length],
    ["Buses", network.fleet.length],
  ];
  $("stats").innerHTML = stats
    .map(([label, value]) => `<div class="stat"><span>${label}</span><strong>${value}</strong></div>`)
    .join("");

  renderLines(network);
  renderFleet(network);
}

function buildLinePaths(network) {
  const stopNames = new Map(network.stops.map((stop) => [stop.id, stop.name]));
  const roadsByLine = new Map(network.lines.map((line) => [line, []]));
  network.roads.forEach((road) => {
    if (!roadsByLine.has(road.line)) roadsByLine.set(road.line, []);
    roadsByLine.get(road.line).push(road);
  });

  return network.lines.map((line) => {
    const roads = roadsByLine.get(line) || [];
    const adjacency = new Map();
    roads.forEach((road) => {
      addEdge(adjacency, road.from, road.to, road);
      addEdge(adjacency, road.to, road.from, road);
    });

    const endpointIds = [...adjacency.entries()]
      .filter(([, edges]) => edges.length === 1)
      .map(([id]) => id)
      .sort((a, b) => a - b);
    const start = endpointIds[0] ?? [...adjacency.keys()].sort((a, b) => a - b)[0];
    if (start == null) return { line, stops: [], segments: [] };

    const stops = [{ id: start, name: stopNames.get(start) || `Stop ${start}` }];
    const segments = [];
    const visited = new Set();
    let current = start;
    let previous = null;

    while (current != null) {
      const edge = (adjacency.get(current) || []).find(
        (candidate) => candidate.other !== previous && !visited.has(edgeKey(current, candidate.other))
      );
      if (!edge) break;
      visited.add(edgeKey(current, edge.other));
      segments.push({ ...edge.road, from: current, to: edge.other });
      previous = current;
      current = edge.other;
      stops.push({ id: current, name: stopNames.get(current) || `Stop ${current}` });
    }

    return { line, stops, segments };
  });
}

function addEdge(map, from, to, road) {
  if (!map.has(from)) map.set(from, []);
  map.get(from).push({ other: to, road });
}

function edgeKey(a, b) {
  return a < b ? `${a}-${b}` : `${b}-${a}`;
}

function renderLines(network) {
  const paths = buildLinePaths(network);
  const counts = new Map();
  paths.forEach((path) => {
    path.stops.forEach((stop) => counts.set(stop.id, (counts.get(stop.id) || 0) + 1));
  });
  const transfers = new Set([...counts.entries()].filter(([, count]) => count > 1).map(([id]) => id));

  $("lineList").innerHTML = paths
    .map((path) => {
      const color = lineColor(path.line);
      const items = [];
      path.stops.forEach((stop, index) => {
        const isTransfer = transfers.has(stop.id);
        items.push(`
          <div class="line-stop ${isTransfer ? "transfer" : ""}">
            <div class="dot"></div>
            <span>${escapeHtml(displayName(stop.name))}${isTransfer ? "<br><em>transfer</em>" : ""}</span>
          </div>`);
        if (index < path.segments.length) {
          const segment = path.segments[index];
          items.push(`<div class="line-segment"><small>${Number(segment.distance).toFixed(1)} km</small></div>`);
        }
      });

      return `
        <div class="line-card" style="--line-color:${color}">
          <div class="line-head">
            <div class="line-name"><span class="line-swatch"></span><h3>${escapeHtml(formatLine(path.line))}</h3></div>
            <small>${path.stops.length} stops</small>
          </div>
          <div class="line-track">${items.join("")}</div>
        </div>`;
    })
    .join("");
}

function renderFleet(network) {
  const rows = network.fleet
    .map(
      (bus) => `
        <tr>
          <td>${bus.index}</td>
          <td><strong>${escapeHtml(bus.type)}</strong><span class="sub">${escapeHtml(bus.id)}</span></td>
          <td>${bus.available} / ${bus.capacity}</td>
          <td>Rs. ${Number(bus.fareRate).toFixed(2)}</td>
          <td>${bus.express ? "Express" : "Standard"}</td>
        </tr>`
    )
    .join("");

  $("fleetTable").innerHTML = `
    <thead><tr><th>#</th><th>Bus</th><th>Seats</th><th>Rate</th><th>Class</th></tr></thead>
    <tbody>${rows}</tbody>`;
}

function readPlanBody() {
  return {
    start: Number($("fromStop").value),
    dest: Number($("toStop").value),
    mode: Number($("mode").value),
    hour: Number($("hour").value),
    passenger: Number($("passenger").value),
    bus: Number($("bus").value),
  };
}

function readCompareBody() {
  return {
    start: Number($("compareFrom").value),
    dest: Number($("compareTo").value),
    mode: Number($("compareMode").value),
    hour: Number($("compareHour").value),
    passenger: Number($("comparePassenger").value),
  };
}

async function onPlan(event) {
  event.preventDefault();
  const button = $("planForm").querySelector('button[type="submit"]');
  await withBusy(button, async () => {
    const body = readPlanBody();
    if (body.start === body.dest) {
      showToast("Choose two different stops.", "error");
      return;
    }
    try {
      const data = await api("/api/plan", body);
      state.lastPlan = { body, plan: data.plan };
      renderPlan(data.plan);
      $("bookCard").classList.remove("hidden");
    } catch (error) {
      showToast(error.message, "error");
    }
  });
}

function renderPlan(plan) {
  const metrics = [
    ["Distance", `${Number(plan.distance).toFixed(2)} km`],
    ["Estimated time", `${Number(plan.minutes).toFixed(1)} min`],
    ["Transfers", plan.transfers],
    ["Estimated fare", `Rs. ${Number(plan.fare).toFixed(2)}`],
  ];

  $("planResult").innerHTML = `
    <div class="summary-grid">
      ${metrics
        .map(
          ([label, value]) => `
            <div class="summary-item"><span>${label}</span><strong>${value}</strong></div>`
        )
        .join("")}
    </div>

    <div class="card trip-card">
      <div class="trip-head">
        <div>
          <h3>${escapeHtml(displayName(plan.origin))} → ${escapeHtml(displayName(plan.destination))}</h3>
          <div class="trip-meta">
            <span class="chip">${escapeHtml(plan.mode)}</span>
            <span class="chip">${escapeHtml(plan.bus)}</span>
            <span class="chip">${escapeHtml(plan.passengerType)}</span>
            <span class="chip rush ${String(plan.rushLabel).toLowerCase()}">${escapeHtml(plan.rushLabel)} ×${Number(plan.rush).toFixed(2)}</span>
            <span class="chip">${plan.availableSeats}/${plan.capacity} seats</span>
          </div>
        </div>
        <span class="tag">${escapeHtml(plan.fareStrategy)}</span>
      </div>
      <div class="timeline">${renderTimeline(plan)}</div>
    </div>

    <div id="ticketSlot"></div>`;
}

function renderTimeline(plan) {
  return plan.stops
    .map((stop, index) => {
      const incoming = index > 0 ? plan.legs[index - 1] : null;
      const outgoing = index < plan.legs.length ? plan.legs[index] : null;
      const line = outgoing?.line || incoming?.line;
      const color = line ? lineColor(line) : "#34d399";
      const transfer = Boolean(incoming && outgoing && incoming.line !== outgoing.line);

      return `
        <div class="timeline-stop ${String(stop.tag).toLowerCase()}" style="--line-color:${color}">
          <div class="timeline-marker"></div>
          <div class="timeline-content">
            <div class="timeline-title">
              <strong>${escapeHtml(displayName(stop.name))}</strong>
              <span class="tag ${String(stop.tag).toLowerCase()}">${escapeHtml(stop.tag)}</span>
            </div>
            ${
              outgoing
                ? `<div class="timeline-leg">
                    <span class="line-badge" style="--line-color:${lineColor(outgoing.line)}">${escapeHtml(formatLine(outgoing.line))}</span>
                    ${Number(outgoing.distance).toFixed(1)} km · ${Number(outgoing.speed).toFixed(0)} km/h
                  </div>`
                : ""
            }
            ${transfer ? `<div class="transfer-note">Change here to ${escapeHtml(formatLine(outgoing.line))}</div>` : ""}
          </div>
        </div>`;
    })
    .join("");
}

async function onBook() {
  if (!state.lastPlan) {
    showToast("Plan a journey first.", "error");
    return;
  }
  const nameInput = $("passengerName");
  const name = nameInput.value.trim();
  if (!name) {
    showToast("Enter a passenger name.", "error");
    nameInput.focus();
    return;
  }

  const button = $("bookButton");
  await withBusy(button, async () => {
    try {
      const data = await api("/api/book", { ...state.lastPlan.body, name });
      renderTicket(data.ticket);
      nameInput.value = "";
      showToast(`Ticket ${data.ticket.id} booked successfully.`, "success");
      await loadNetwork({ populate: false });
    } catch (error) {
      showToast(error.message, "error");
    }
  });
}

function renderTicket(ticket) {
  const slot = $("ticketSlot");
  if (!slot) return;

  slot.innerHTML = `
    <div class="card ticket-card">
      <div class="ticket-head">
        <span>Booking confirmed</span>
        <strong>${escapeHtml(ticket.id)}</strong>
      </div>
      <div class="ticket-grid">
        <div><span>Passenger</span><strong>${escapeHtml(ticket.passenger)} (${escapeHtml(ticket.passengerType)})</strong></div>
        <div><span>Bus</span><strong>${escapeHtml(ticket.bus)}</strong></div>
        <div><span>Route</span><strong>${escapeHtml(displayName(ticket.route))}</strong></div>
        <div><span>Distance</span><strong>${Number(ticket.distance).toFixed(2)} km</strong></div>
        <div><span>ETA</span><strong>${Number(ticket.minutes).toFixed(1)} min</strong></div>
        <div><span>Fare</span><strong>Rs. ${Number(ticket.fare).toFixed(2)} · ${escapeHtml(ticket.fareStrategy)}</strong></div>
      </div>
    </div>`;
}

async function onCompare(event) {
  event.preventDefault();
  const button = $("compareForm").querySelector('button[type="submit"]');
  await withBusy(button, async () => {
    const body = readCompareBody();
    if (body.start === body.dest) {
      showToast("Choose two different stops.", "error");
      return;
    }
    try {
      const data = await api("/api/compare", body);
      renderCompare(data.compare);
    } catch (error) {
      showToast(error.message, "error");
    }
  });
}

function renderCompare(compare) {
  const found = compare.options.filter((option) => option.found);
  const bestTime = found.length ? Math.min(...found.map((option) => Number(option.minutes))) : null;
  const bestFare = found.length ? Math.min(...found.map((option) => Number(option.fare))) : null;

  const rows = compare.options
    .map((option) => {
      if (!option.found) {
        return `
          <tr>
            <td><strong>${escapeHtml(option.bus)}</strong><span class="sub">${escapeHtml(option.busId)}</span></td>
            <td colspan="5" class="muted">No route available</td>
          </tr>`;
      }

      const minutes = Number(option.minutes);
      const fare = Number(option.fare);
      return `
        <tr>
          <td><strong>${escapeHtml(option.bus)}</strong><span class="sub">${escapeHtml(option.busId)}</span></td>
          <td>${Number(option.distance).toFixed(1)} km</td>
          <td class="${minutes === bestTime ? "best" : ""}">${minutes.toFixed(1)} min</td>
          <td>${option.transfers}</td>
          <td class="${fare === bestFare ? "best" : ""}">Rs. ${fare.toFixed(2)}</td>
          <td>${escapeHtml(option.strategy)}</td>
        </tr>`;
    })
    .join("");

  $("compareResult").innerHTML = `
    <div class="summary-grid">
      <div class="summary-item"><span>Mode</span><strong>${escapeHtml(compare.mode)}</strong></div>
      <div class="summary-item"><span>Hour</span><strong>${String(compare.hour).padStart(2, "0")}:00</strong></div>
      <div class="summary-item"><span>Rush</span><strong>×${Number(compare.rush).toFixed(2)}</strong></div>
      <div class="summary-item"><span>Passenger</span><strong>${escapeHtml(compare.passengerType)}</strong></div>
    </div>

    <div class="card">
      <div class="card-head"><h3>Bus options</h3><span class="muted">Green = best value</span></div>
      <div class="table-wrap">
        <table class="compare-table">
          <thead>
            <tr><th>Bus</th><th>Distance</th><th>Time</th><th>Transfers</th><th>Fare</th><th>Strategy</th></tr>
          </thead>
          <tbody>${rows}</tbody>
        </table>
      </div>
    </div>`;
}

async function withBusy(button, task) {
  if (!button) return task();
  const label = button.textContent;
  button.disabled = true;
  button.textContent = "Working...";
  try {
    await task();
  } finally {
    button.disabled = false;
    button.textContent = label;
  }
}

function setStatus(text, stateName) {
  $("statusText").textContent = text;
  $("backendStatus").dataset.state = stateName;
}

let toastTimer = null;

function showToast(message, type = "info") {
  const toast = $("toast");
  toast.textContent = message;
  toast.className = `toast show ${type}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => {
    toast.className = "toast hidden";
  }, 3800);
}

function lineColor(line) {
  if (LINE_COLORS[line]) return LINE_COLORS[line];
  let hash = 0;
  for (let index = 0; index < line.length; index += 1) {
    hash = (hash * 31 + line.charCodeAt(index)) | 0;
  }
  return FALLBACK_LINE_COLORS[Math.abs(hash) % FALLBACK_LINE_COLORS.length];
}

function formatLine(line) {
  return String(line).replace(/([a-z])([A-Z])/g, "$1 $2");
}

function displayName(name) {
  return String(name).replaceAll("_", " ");
}

function escapeHtml(value) {
  return String(value).replace(
    /[&<>"']/g,
    (character) =>
      ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;",
      }[character])
  );
}
