//
// Created by Eric Vachon on 2019-06-11.
//
// Simple wrapper for the project, should create a non-blocking tcpSocketWrapper with (mostly) consistent behavior across platforms.
//
//
//


#ifndef TLS_TCPSOCKETWRAPPER_H
#define TLS_TCPSOCKETWRAPPER_H

#include <Winsock2.h>

#include <variant>
#include <memory>

namespace tcp{

class tcpSocketWrapper {
    friend class initializedState;
    friend class connectingState;
    friend class connectedState;

public:
    void Connect(const char* hostAddress, const char* service);
    void Write();
    void Read();
    void Update();

    tcpSocketWrapper();
    ~tcpSocketWrapper();

private:
    SOCKET tcpSocket = INVALID_SOCKET;
    const int addressProtocol = AF_INET6;
    const int socketType = SOCK_STREAM;
    const int socketProtocol = IPPROTO_TCP;
    const LPWSAPROTOCOL_INFOW socketProtocolInfo = NULL;
    const DWORD socketFlags = WSA_FLAG_OVERLAPPED;

    std::unique_ptr<socketState> currentState;
};

class socketState {
public:
    virtual std::unique_ptr<socketState> Connect(const char *, const char *)=0;
    virtual std::unique_ptr<socketState> Write()=0;
    virtual std::unique_ptr<socketState> Read()=0;
    virtual std::unique_ptr<socketState> Update()=0;

    virtual ~socketState()=0;
protected:
    socketState(tcpSocketWrapper &socket);
    tcpSocketWrapper &socketWrapper;
};

class initializedState: public socketState{
    friend class connectingState;
public:
    initializedState(tcpSocketWrapper &socket);

    std::unique_ptr<socketState> Connect(const char *hostAddress, const char *service);
    std::unique_ptr<socketState> Write();
    std::unique_ptr<socketState> Read();
    std::unique_ptr<socketState> Update();

    ~initializedState();
};

class connectingState: public socketState{
public:
    connectingState(initializedState&, addrinfo*);

    std::unique_ptr<socketState> Connect(const char*, const char*);
    std::unique_ptr<socketState> Write();
    std::unique_ptr<socketState> Read();
    std::unique_ptr<socketState> Update();

    ~connectingState();
private:
    WSAEVENT connectEvent;
};

class connectedState: public socketState {
public:
    connectedState(initializedState &socket);

    std::unique_ptr<socketState> Connect(const char *hostAddress, const char *service);

    std::unique_ptr<socketState> Write();

    std::unique_ptr<socketState> Read();

    std::unique_ptr<socketState> Update();

    ~connectedState();
}
};

#endif //TLS_TCPSOCKETWRAPPER_H
