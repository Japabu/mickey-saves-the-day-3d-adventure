"""Serves the web build locally with the headers shared memory needs (COOP/COEP).

usage: python3 port/web/serve.py [directory] [port]   (default out/port/build-web, 8000)
then open http://localhost:8000/
"""
import functools
import http.server
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {**http.server.SimpleHTTPRequestHandler.extensions_map,
                      '.wasm': 'application/wasm', '.js': 'text/javascript'}

    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cache-Control', 'no-store')
        super().end_headers()


def main():
    directory = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'out', 'port', 'build-web')
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8000
    server = http.server.ThreadingHTTPServer(('localhost', port), functools.partial(Handler, directory=directory))
    print('serving %s on http://localhost:%d/' % (directory, port))
    server.serve_forever()


if __name__ == '__main__':
    main()
