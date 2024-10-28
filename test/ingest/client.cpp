#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

const char* SERVER_IP = "127.0.0.1";            // Server address (localhost)
const int PORT = 8080;                          // Server port

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};
    
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::string message = "Client sends message";
    send(clientSocket, message.c_str(), message.size(), 0);
    std::cout << "Message sent to server: " << message << std::endl;

    ssize_t bytesRead = read(clientSocket, buffer, 1024);
    if (bytesRead > 0) {
        std::cout << "Server response: " << buffer << std::endl;
    }

    close(clientSocket);
    return 0;
}
