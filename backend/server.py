# backend/server.py
import ctypes
import os
from http.server import BaseHTTPRequestHandler, HTTPServer

# Load the shared library (dll on Windows, so on Linux)
lib_ext = "dll" if os.name == "nt" else "so"
lib_name = f"./ministream.{lib_ext}"

if not os.path.exists(lib_name):
    print(f"Error: {lib_name} not found! Compile it first using zig cc -shared ...")
    exit(1)

lib = ctypes.CDLL(lib_name)
lib.deney_json.restype = ctypes.c_char_p

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/benchmark':
            sonuc = lib.deney_json()
            if sonuc is None:
                # Fallback if C function failed
                sonuc = b'{"error": "Failed to generate JSON"}'
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(sonuc)
            
    def log_message(self, format, *args):
        pass # terminal ciktisini sustur

print("Server is listening on http://localhost:8765")
HTTPServer(('localhost', 8765), Handler).serve_forever()
