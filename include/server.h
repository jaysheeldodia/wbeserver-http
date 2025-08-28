#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "http_request.h"
#include "file_handler.h"

class WebServer {
private:
    int server_fd;
    int port;
    struct sockaddr_in address;
    std::string document_root;
    FileHandler* file_handler;

public:
    WebServer(int port = 8080, const std::string& doc_root = "./www");
    ~WebServer();
    
    bool initialize();
    void start();
    void handle_client(int client_socket);
    void cleanup();
    
private:
    std::string read_request(int client_socket);
    void send_response(int client_socket, const std::string& response);
    std::string build_http_response(int status_code, const std::string& status_text, 
                                  const std::string& content_type, const std::string& body);
    std::string handle_get_request(const HttpRequest& request);
    std::string get_404_response();
};

#endif // SERVER_H