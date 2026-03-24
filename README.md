# HTTP-Server-In-C-

A simple and lightweight HTTP server written in C.

## Features

- TCP socket creation
- Binding to a port
- Listening for incoming connections
- Accepting client connections
- Receiving HTTP requests into a buffer
- HTTP request parsing (request line and headers)
- Parse result handling (`200`, `400`, `413`, `505`, fallback `500`)
- Sending HTTP responses

## Usage

Compile on Linux:

```bash
gcc -Wall -Wextra -pedantic src/main.c src/socket_io.c src/http_response.c src/parser.c -o server
./server
```

Compile on Windows with MinGW:

```bash
gcc -Wall -Wextra -pedantic src/main.c src/socket_io.c src/http_response.c src/parser.c -o server -lws2_32
server.exe
```

Then open `http://localhost:8000` in a browser or use `curl http://127.0.0.1:8000`.

## Notes

- This server is minimal and intended for learning, not production use.
- It currently supports basic HTTP request receipt and sending a fixed HTML response.
- The code now builds on both Windows and Linux.

## License

This project is released under the MIT License.
