#include <iostream>
#include <thread>
#include <csignal>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
int serverSocket;

void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    
    ssize_t bytesRead = read(clientSocket, buffer, 1024);
    if (bytesRead > 0) {
        std::cout << "Received message from client: " << buffer << std::endl;
        
        std::string response = "Message received!";
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
    std::cout << "Client disconnected." << std::endl;
}

void handleSigint(int sig) {
    std::cout << "\nReceived SIGINT (Ctrl+C), closing socket and exiting..." << std::endl;
    // Close the server socket
    if (serverSocket >= 0) {
        close(serverSocket);
    }
    exit(0);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    signal(SIGINT, handleSigint);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0) {
        perror("Socket failed");
        return 1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Set socket options failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed");
        close(serverSocket);
        return 1;
    }
    
    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }
        
        std::cout << "New client connected." << std::endl;
        
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}
