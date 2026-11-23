#!/usr/bin/env python3
"""Local web UI server for the Pune Smart Bus Route Planner.

The HTML/CSS/JS frontend lives in web_ui/. Requests are forwarded to a
persistent C++ backend process running in --api mode, so the existing routing,
fare, booking and file-handling logic is reused rather than duplicated.
"""

from __future__ import annotations

import atexit
import json
import os
import subprocess
import sys
import threading
import time
import webbrowser
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent
WEB_DIR = BASE_DIR / "web_ui"
SOURCE = BASE_DIR / "OOP_CP.cpp"
API_EXE = BASE_DIR / "build" / "pune_bus_api.exe"
PORT = int(os.environ.get("PORT", "8000"))

api_process: subprocess.Popen[str] | None = None
api_lock = threading.Lock()


def build_api() -> None:
    if not SOURCE.exists():
        sys.exit(f"Missing source file: {SOURCE}")

    needs_build = (
        not API_EXE.exists()
        or API_EXE.stat().st_mtime < SOURCE.stat().st_mtime
    )
    if not needs_build:
        return

    print("Compiling C++ backend...")
    API_EXE.parent.mkdir(parents=True, exist_ok=True)
    try:
        subprocess.run(
            [
                "g++",
                "-std=c++17",
                "-O2",
                "-Wall",
                "-Wextra",
                "-o",
                str(API_EXE),
                str(SOURCE),
            ],
            cwd=BASE_DIR,
            check=True,
        )
    except FileNotFoundError:
        sys.exit("g++ was not found. Install MinGW/MSYS2 and add g++ to PATH.")
    except subprocess.CalledProcessError as exc:
        sys.exit(f"C++ backend compilation failed (exit code {exc.returncode}).")
    print(f"Backend built: {API_EXE}")


def start_api() -> None:
    global api_process
    build_api()
    api_process = subprocess.Popen(
        [str(API_EXE), "--api"],
        cwd=BASE_DIR,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
    )
    if api_process.stdin is None or api_process.stdout is None:
        raise RuntimeError("Could not open pipes to the C++ backend.")

    response = call_api({"action": "network"})
    if not response.get("ok"):
        raise RuntimeError(response.get("error", "C++ backend failed to start."))
    print("C++ backend ready.")


def stop_api() -> None:
    global api_process
    process = api_process
    api_process = None
    if process is None:
        return
    if process.poll() is None:
        try:
            if process.stdin:
                process.stdin.close()
            process.terminate()
            process.wait(timeout=3)
        except Exception:
            process.kill()


def call_api(payload: dict) -> dict:
    process = api_process
    if process is None or process.stdin is None or process.stdout is None:
        raise RuntimeError("C++ backend is not running.")

    with api_lock:
        process.stdin.write(json.dumps(payload, separators=(",", ":")) + "\n")
        process.stdin.flush()
        line = process.stdout.readline()
        if not line:
            stderr = ""
            if process.poll() is not None and process.stderr is not None:
                stderr = process.stderr.read().strip()
            raise RuntimeError("C++ backend stopped unexpectedly." + (f" {stderr}" if stderr else ""))
        return json.loads(line)


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB_DIR), **kwargs)

    def do_GET(self) -> None:
        path = self.path.split("?", 1)[0]
        if path == "/api/network":
            self.handle_api({"action": "network"})
            return
        super().do_GET()

    def do_POST(self) -> None:
        path = self.path.split("?", 1)[0]
        actions = {
            "/api/plan": "plan",
            "/api/compare": "compare",
            "/api/book": "book",
        }
        action = actions.get(path)
        if action is None:
            self.send_error(404, "Unknown API endpoint")
            return

        length = int(self.headers.get("Content-Length", "0"))
        raw = self.rfile.read(length) if length > 0 else b"{}"
        try:
            payload = json.loads(raw)
        except json.JSONDecodeError:
            self.write_json({"ok": False, "error": "Invalid JSON request."}, 400)
            return
        if not isinstance(payload, dict):
            self.write_json({"ok": False, "error": "JSON request must be an object."}, 400)
            return

        payload["action"] = action
        self.handle_api(payload)

    def handle_api(self, payload: dict) -> None:
        try:
            result = call_api(payload)
            self.write_json(result, 200 if result.get("ok") else 400)
        except Exception as exc:  # noqa: BLE001 - surface backend errors to the UI
            self.write_json({"ok": False, "error": str(exc)}, 500)

    def write_json(self, payload: dict, status: int = 200) -> None:
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt: str, *args) -> None:
        return


def main() -> None:
    if not WEB_DIR.exists():
        sys.exit(f"Missing UI directory: {WEB_DIR}")

    atexit.register(stop_api)
    start_api()

    try:
        server = ThreadingHTTPServer(("127.0.0.1", PORT), Handler)
    except OSError as exc:
        sys.exit(f"Could not start server on port {PORT}: {exc}")

    url = f"http://127.0.0.1:{PORT}"
    print(f"UI running at {url}")
    print("Press Ctrl+C to stop.")
    if os.environ.get("NO_BROWSER") != "1":
        threading.Timer(0.8, lambda: webbrowser.open(url)).start()

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        server.server_close()
        stop_api()


if __name__ == "__main__":
    main()
