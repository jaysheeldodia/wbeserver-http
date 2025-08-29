#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <unordered_map>
#include "http_request.h"
#include "file_handler.h"
#include "thread_pool.h"

class WebServer {
private:
    int server_fd;
    int port;
    struct sockaddr_in address;
    std::string document_root;
    FileHandler* file_handler;
    ThreadPool* thread_pool;
    
    bool keep_alive_enabled; // Flag for enabling keep-alive
    std::chrono::seconds connection_timeout; // Timeout duration for idle connections
    std::unordered_map<int, std::chrono::steady_clock::time_point> connection_timestamps; // Track connection activity

public:
    WebServer(int port = 8080, const std::string& doc_root = "./www", size_t thread_count = 4);
    ~WebServer();
    
    bool initialize();
    void start();
    void handle_client(int client_socket);
    void handle_client_task(int client_socket);  // Thread-safe version
    void cleanup();
    
    void enable_keep_alive(bool enable, int timeout_seconds = 5); // Enable/disable keep-alive
    void manage_connections(); // Manage connection timeouts
    
private:
    std::string read_request(int client_socket);
    void send_response(int client_socket, const std::string& response);
    std::string build_http_response(int status_code, const std::string& status_text, 
                                  const std::string& content_type, const std::string& body, 
                                  const std::string& connection = "close");
    std::string handle_get_request(const HttpRequest& request);
    std::string get_404_response();
};

#endif // SERVER_H