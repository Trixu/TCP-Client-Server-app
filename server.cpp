#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;
//
// args: port, ip_addr, max_number_of_connections
//
//
class Server
{
public:
    Server(int port, string addr, int maxConnections)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        if (inet_pton(AF_INET, addr.c_str(), &server.sin_addr) < 1)
            throw runtime_error("Bad address");

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            throw runtime_error("Socket error");
        
        len = sizeof(server);

        if (bind(serverSocket, (struct sockaddr*) &server, len) == -1)
            throw runtime_error("Bind error");
        
        if (listen(serverSocket, maxConnections) == -1)
            throw runtime_error("listen error");
    }

    int acceptClient()
    {
        clientSocket = accept(serverSocket, (struct sockaddr*) &client, &len);
        if (clientSocket == -1)
        {
            throw runtime_error("accept() error");
        }
        return 1;
    }

    void sendToClient(string msg)
    {
        if (send(clientSocket, msg.c_str(), 4096, 0) == -1)
        {
            throw runtime_error("send() error");
        }
        
    }

    void receive()
    {
        if (recv(clientSocket, buffer, 4096, 0) == -1)
        {
            throw runtime_error("recv() error");
        }
    }

    void printMsg()
    {
        cout << buffer << endl;
    }

    void conShutdown()
    {
        shutdown(clientSocket, SHUT_RDWR);
    }

    void servShutdown()
    {
        shutdown(serverSocket, SHUT_RDWR);
    }

    void start()
    {
        cout << "Server is running!" << endl << endl;
        cout << "Waiting for connection" << endl;

        if (acceptClient())
        {
            cout << "połączony" << endl;
            sendToClient("Send \"1\" if you want upload something or \"q\" if you want to close cennetion\n");
        }
        receive();

        if (!(strcmp(buffer, "1")))
        {
            sendToClient("waiting");
            while (1)
            {
                receive();
                if (!(strcmp(buffer, "q")))
                {
                    sendToClient("shut");
                    conShutdown();
                    break;
                }
                printMsg();
                sendToClient("waiting");
            }
        }
        else if (!(strcmp(buffer, "q")))
        {
            sendToClient("shut");
            conShutdown();
        }
        else
        {
            sendToClient("shut");
            conShutdown();
        }

        servShutdown();
    }

private:
    struct sockaddr_in server;
    struct sockaddr_in client;
    int serverSocket;
    int clientSocket;
    char buffer[4096] = {};
    string strBuffer;
    socklen_t len;
};

int main(int argc, char const *argv[])
{
    try
    {
    Server server(stoi(argv[1]), argv[2], stoi(argv[3]));
    server.start();
    }
    catch(const exception e)
    {
        cout << "Error: " << e.what() << endl;
    }
    return 0;
}
