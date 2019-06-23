//
// Created by Eric Vachon on 2019-06-11.
//
// Simple wrapper for the project, should create a non-blocking tcpWrapper with (mostly) consistent behavior across platforms.
//
//
//


#ifndef TLS_TCPWRAPPER_H
#define TLS_TCPWRAPPER_H

#include <Winsock2.h>

#include <variant>
#include <memory>

namespace tcp{

class tcpWrapper {
    friend class initializedState;
    friend class connectingState;
    friend class connectedState;

public:
    void Connect(const char* hostAddress, const char* service);
    void Write(const char*, int);
    void Read(const char*, int);
    void Update();

    /**
     * Initializes the windows socket wrapper and creates an unconnected socket.
     * Initiates the use of the windows socket .dll by the application and initiates the socket. Sets the socket to
     * have a preference for IPv6 addresses and be non-blocking.
     * @throws fatalWSAException In the case that WSAStartup fails for any reason.
     * @throws fatalSocketException In the case that socket creation or option setting fails for any reason.
     */
    tcpWrapper();
    /**
     * Deconstructor for windows socket wrapper, destroys any sockets and closes the winsock2 .dll
     * Will print errors to std::cout for logging, but will not throw any errors.
     */
    ~tcpWrapper();

private:
    // Constant values used throughout the code
    const WORD version = MAKEWORD(2,2);
    const int addressProtocol = AF_INET6;
    const int socketType = SOCK_STREAM;
    const int socketProtocol = IPPROTO_TCP;
    const LPWSAPROTOCOL_INFOW socketProtocolInfo = NULL;
    const DWORD socketFlags = WSA_FLAG_OVERLAPPED;

    // Actual variables
    SOCKET tcpSocket = INVALID_SOCKET;
    std::unique_ptr<socketState> currentState;
};

class socketState {
public:
    virtual void Connect(tcpWrapper&, const char *, const char *)=0;
    virtual void Write()=0;
    virtual void Read()=0;
    virtual void Update()=0;

    virtual ~socketState()=0;
};

class initializedState: public socketState{
public:
    void Connect(tcpWrapper&,const char *hostAddress, const char *service);
    void Write();
    void Read();
    void Update();

    ~initializedState();
};

class connectingState: public socketState{
public:
    connectingState(tcpWrapper&, addrinfo*);

    void Connect(tcpWrapper&,const char*, const char*);
    void Write();
    void Read();
    void Update();

    ~connectingState();
private:
    WSAEVENT connectEvent;
    struct _OVERLAPPED connectingOverlap;
};

class connectedState: public socketState {
public:
    connectedState(initializedState &socket);

    void Connect(tcpWrapper&,const char *hostAddress, const char *service);
    void Write();
    void Read();
    void Update();

    ~connectedState();
}
};

#endif //TLS_TCPWRAPPER_H
