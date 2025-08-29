# wbeserver-http

## Overview
`wbeserver-http` is a lightweight HTTP web server designed to handle multiple client requests efficiently. It is implemented in C++ and includes features such as multithreading, file handling, and HTTP request parsing. The server is capable of serving static files like HTML, CSS, and JSON.

## Project Structure

```
load_test.cpp
load_tester
Makefile
README.md
webserver
build/
	file_handler.o
	http_request.o
	main.o
	server.o
	thread_pool.o
include/
	file_handler.h
	http_request.h
	server.h
	thread_pool.h
src/
	file_handler.cpp
	http_request.cpp
	main.cpp
	server.cpp
	thread_pool.cpp
tests/
www/
	about.html
	data.json
	index.html
	style.css
```

### Key Directories and Files
- **`src/`**: Contains the main source code files for the server implementation.
- **`include/`**: Header files for the server components.
- **`build/`**: Compiled object files.
- **`www/`**: Static files served by the web server, such as HTML, CSS, and JSON.
- **`tests/`**: Directory for test cases.
- **`Makefile`**: Build configuration for compiling the project.

## Features
- **Multithreading**: Efficiently handles multiple client requests using a thread pool.
- **File Handling**: Serves static files from the `www/` directory.
- **HTTP Request Parsing**: Processes HTTP requests and generates appropriate responses.

## Build Instructions
1. Ensure you have `g++` installed on your system.
2. Run the following command to build the project:
   ```bash
   make
   ```
3. The compiled binary will be available as `webserver`.

## Run Instructions
1. Start the server by running:
   ```bash
   ./webserver
   ```
2. Access the server in your browser at `http://localhost:8080` (default port).

## Testing
- The `tests/` directory contains test cases for the server. You can run the tests using the `load_tester` binary.

## WebServer Class

The `WebServer` class is the core of the `wbeserver-http` project. It is responsible for managing client connections, serving files, and handling HTTP requests.

### Key Features
- **Keep-Alive Support**: The server supports persistent connections with configurable timeouts.
- **Thread Pool**: Efficiently handles multiple client requests using a thread pool.
- **Dynamic Configuration**: Allows configuration of port, document root, and thread count via command-line arguments.

### Public Methods
- `WebServer(int port, const std::string& doc_root, size_t thread_count)`: Constructor to initialize the server.
- `~WebServer()`: Destructor to clean up resources.
- `bool initialize()`: Initializes the server socket and prepares it to accept connections.
- `void start()`: Starts the server and begins accepting client connections.
- `void handle_client(int client_socket)`: Handles a single client connection.
- `void enable_keep_alive(bool enable, int timeout_seconds)`: Enables or disables keep-alive functionality.
- `void manage_connections()`: Manages connection timeouts for idle clients.
- `void cleanup()`: Cleans up resources and shuts down the server.

### Signal Handling
The server includes signal handling for graceful shutdown. Signals such as `SIGINT` and `SIGTERM` are captured to ensure proper cleanup of resources.

### Example Usage
To start the server with default settings:
```bash
./webserver
```

To specify custom settings:
```bash
./webserver <port> <document_root> <thread_count>
```
For example:
```bash
./webserver 8081 ./public 8
```

## HTTP Request Handling
The `HttpRequest` class is used to parse and validate incoming HTTP requests. It supports methods such as `GET` and provides utilities for debugging and logging requests.

## File Handling
The `FileHandler` class is responsible for serving static files. It includes features such as:
- Resolving file paths.
- Determining MIME types.
- Reading file contents.

Supported MIME types include:
- `text/html`
- `text/css`
- `application/javascript`
- `image/png`, `image/jpeg`, etc.

## Load Testing
The `load_test.cpp` file includes a `LoadTester` class for benchmarking the server. It provides metrics such as:
- Total time taken.
- Successful and failed requests.
- Requests per second.
- Response time statistics.

To run the load tester:
```bash
./load_tester <server_url>
```

## Static Files
The `www/` directory contains sample static files:
- `index.html`: Welcome page.
- `about.html`: About page.
- `style.css`: Stylesheet for the HTML pages.
- `data.json`: Sample JSON data.

You can add your own files to this directory to extend the server's functionality.

## Contributing
Contributions are welcome! Feel free to submit issues or pull requests to improve the project.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.
