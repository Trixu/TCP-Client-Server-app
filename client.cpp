#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>


using namespace std;

class Client
{
public:
    Client(int port, string addr)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        if (inet_pton(AF_INET, addr.c_str(), &server.sin_addr) < 1)
            throw runtime_error("Bad address");

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            throw runtime_error("Socket error");

        len = sizeof(server);
    }

    int connectSv()
    {
        if (connect(serverSocket, (struct sockaddr*) &server, len) == -1)
        {
            throw runtime_error("connection error");
        }
        return 1;
    }

    void sendBuffer()
    {
        cin >> buffer;
        if (send(serverSocket, buffer.c_str(), 4096, 0) == -1)
        {
            throw runtime_error("send() error");
        }
        
    }

    void receive()
    {
        if (recv(serverSocket, recvBuffer, 4096, 0) == -1)
        {
            throw runtime_error("recv() error");
        }
    }

    int conShutdown()
    {
        if (shutdown(serverSocket, SHUT_RDWR) == -1)
        {
            throw runtime_error("shutdown() error");
        }
        return 0;
    }

    void start()
    {
        int connection = 0;
        cout << "Waiting for connection" << endl;
        if (connectSv())
        {
            cout << "Connected succefully :)" << endl;;
            connection = 1;
        }

        receive();
        cout << recvBuffer;

        sendBuffer();
        receive();

        while (!(strcmp(recvBuffer, "waiting")))
        {
            sendBuffer();
            receive();
        }
        cout << "Disconeccting" << endl;
        conShutdown();
    }

private:
    struct sockaddr_in server;
    int serverSocket;
    string buffer;
    char recvBuffer[4096] = {};
    socklen_t len;
};

int main(int argc, char const *argv[])
{
    try
    {
    Client client(stoi(argv[1]), argv[2]);
    client.start();
    }
    catch(const exception e)
    {
        cout << "Error: " << e.what() << endl;;
    }
    return 0;
}
