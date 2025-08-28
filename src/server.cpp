#include "../include/server.h"
#include <iostream>
#include <cstring>
#include <errno.h>

WebServer::WebServer(int port, const std::string& doc_root) 
    : server_fd(-1), port(port), document_root(doc_root) {
    memset(&address, 0, sizeof(address));
    file_handler = new FileHandler(document_root);
}

WebServer::~WebServer() {
    cleanup();
    delete file_handler;
}

bool WebServer::initialize() {
    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Configure address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    address.sin_port = htons(port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Start listening for connections
    if (listen(server_fd, 10) < 0) {  // Backlog of 10 connections
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Server initialized on port " << port << std::endl;
    return true;
}

void WebServer::start() {
    std::cout << "Server starting on http://localhost:" << port << std::endl;
    std::cout << "Document root: " << document_root << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        // Accept incoming connection
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Handle the client request
        handle_client(client_socket);
        
        // Close client socket
        close(client_socket);
    }
}

void WebServer::handle_client(int client_socket) {
    // Read the request
    std::string request_data = read_request(client_socket);
    
    if (request_data.empty()) {
        std::cout << "Empty request received" << std::endl;
        return;
    }

    // Parse HTTP request
    HttpRequest request;
    if (!request.parse(request_data)) {
        std::cout << "Failed to parse HTTP request" << std::endl;
        std::string response = build_http_response(400, "Bad Request", 
                                                 "text/html", 
                                                 "<html><body><h1>400 Bad Request</h1></body></html>");
        send_response(client_socket, response);
        return;
    }

    // Debug: print request details
    std::cout << "Request: " << request.method << " " << request.path << std::endl;

    // Handle different HTTP methods
    std::string response;
    if (request.method == "GET") {
        response = handle_get_request(request);
    } else {
        // Method not allowed
        response = build_http_response(405, "Method Not Allowed", 
                                     "text/html", 
                                     "<html><body><h1>405 Method Not Allowed</h1></body></html>");
    }

    send_response(client_socket, response);
}

std::string WebServer::read_request(int client_socket) {
    char buffer[4096];
    std::string request;
    
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        request = std::string(buffer);
    } else if (bytes_received == 0) {
        std::cout << "Client closed connection" << std::endl;
    } else {
        std::cerr << "Recv failed: " << strerror(errno) << std::endl;
    }
    
    return request;
}

void WebServer::send_response(int client_socket, const std::string& response) {
    ssize_t bytes_sent = send(client_socket, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Send failed: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent " << bytes_sent << " bytes" << std::endl;
    }
}

void WebServer::cleanup() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
}

std::string WebServer::build_http_response(int status_code, const std::string& status_text, 
                                         const std::string& content_type, const std::string& body) {
    std::string response = "HTTP/1.1 " + std::to_string(status_code) + " " + status_text + "\r\n";
    response += "Content-Type: " + content_type + "\r\n";
    response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    
    return response;
}

std::string WebServer::handle_get_request(const HttpRequest& request) {
    // Check if file exists
    if (!file_handler->file_exists(request.path)) {
        return get_404_response();
    }
    
    // Read file content
    std::string content = file_handler->read_file(request.path);
    if (content.empty()) {
        return get_404_response();
    }
    
    // Get MIME type
    std::string mime_type = file_handler->get_mime_type(request.path);
    
    // Build and return response
    return build_http_response(200, "OK", mime_type, content);
}

std::string WebServer::get_404_response() {
    std::string body = "<html><body><h1>404 Not Found</h1><p>The requested file was not found.</p></body></html>";
    return build_http_response(404, "Not Found", "text/html", body);
}