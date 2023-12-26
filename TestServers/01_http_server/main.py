import http.server
import socketserver

PORT = 8000


handler = http.server.SimpleHTTPRequestHandler
handler.protocol_version = 'HTTP/1.1'


with socketserver.TCPServer( ("", PORT), handler) as httpd:
    print("serving at port", PORT)
    httpd.serve_forever()