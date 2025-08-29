#include "../include/server.h"
#include <iostream>
#include <signal.h>

WebServer* server_instance = nullptr;

// Signal handler for graceful shutdown
void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down gracefully..." << std::endl;
    if (server_instance) {
        server_instance->cleanup();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    // Set up signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int port = 8080;
    std::string doc_root = "./www";
    size_t thread_count = 4;

    // Parse command line arguments
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }
    if (argc > 2) {
        doc_root = argv[2];
    }
    if (argc > 3) {
        thread_count = std::stoi(argv[3]);
    }

    std::cout << "Starting server with:" << std::endl;
    std::cout << "  Port: " << port << std::endl;
    std::cout << "  Document root: " << doc_root << std::endl;
    std::cout << "  Thread count: " << thread_count << std::endl;

    // Create and initialize server
    WebServer server(port, doc_root, thread_count);
    server_instance = &server;

    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    // Start the server (this will run indefinitely)
    try {
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}