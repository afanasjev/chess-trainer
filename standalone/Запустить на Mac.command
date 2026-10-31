#!/bin/bash
cd -- "$(dirname -- "$0")" || exit 1
python3 -u - <<'PY'
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
import socket, subprocess, webbrowser

page = Path('openings.html').read_bytes()
class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.split('?')[0] not in ('/', '/openings.html'):
            self.send_error(404)
            return
        self.send_response(200)
        self.send_header('Content-Type', 'text/html; charset=utf-8')
        self.send_header('Content-Length', str(len(page)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(page)
    def log_message(self, *args): pass

server = None
for port in range(8765, 8776):
    try:
        server = HTTPServer(('0.0.0.0', port), Handler)
        break
    except OSError: pass
if server is None:
    raise SystemExit('Не удалось запустить тренажёр. Закройте прежнее окно запуска и попробуйте снова.')
ip = None
for interface in ('en0', 'en1', 'en2'):
    try:
        value = subprocess.check_output(['/usr/sbin/ipconfig', 'getifaddr', interface], stderr=subprocess.DEVNULL, text=True).strip()
        if value:
            ip = value
            break
    except (OSError, subprocess.SubprocessError): pass
if ip is None:
    ip = socket.gethostname()
    if '.' not in ip: ip += '.local'
print('\nТренажёр дебютов запущен.\n')
print('Откройте в Safari на iPhone:')
print(f'\n    http://{ip}:{port}/\n')
print('iPhone и Mac должны быть в одной сети Wi-Fi.')
print('При запросе macOS разрешите входящие подключения для этого запуска.')
print('Оставьте это окно открытым и не переводите Mac в сон.')
print('Для остановки нажмите Control+C или закройте окно.\n')
webbrowser.open(f'http://localhost:{port}/')
try: server.serve_forever()
except KeyboardInterrupt: pass
finally: server.server_close()
PY
