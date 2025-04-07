from urllib.parse import parse_qs, urlparse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import subprocess


def run(args):
    return subprocess.run(["main.exe"]+args, capture_output=True, text=True).returncode


class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Parse query parameters from the URL
        query = parse_qs(urlparse(self.path).query)
        args = query.get('args', [])

        # Decode arguments if necessary (they will be lists)
        args = [arg for sublist in args for arg in sublist.split(',')]

        # Send the response headers
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

        try:
            # Run the command if args are provided
            if args:
                response = run(args)
            else:
                response = 0

            print(response)

            # Send the JSON response
            self.wfile.write(json.dumps(response).encode('utf-8'))
        except Exception as e:
            # Handle errors
            error_response = 0
            self.wfile.write(json.dumps(error_response).encode('utf-8'))

if __name__ == "__main__":
    host = "0.0.0.0"  # Allow access from any IP
    port = 25565
    server = ThreadingHTTPServer((host, port), RequestHandler)
    print(f"Server running on http://{host}:{port}/")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down the server...")
        server.server_close()
