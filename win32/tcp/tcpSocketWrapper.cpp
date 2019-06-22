/*
 * Created by Eric Vachon on 2019-06-11.
 *
 * Very basic wrapper to hopefully keep different platforms happy. This allows for a simple TCP socket to be
 * created on windows machines. The socket will always have the non-blocking and duplex flags set (i.e. IPv4 and IPv6).
*/

#include "tcpSocketWrapper.h"

#include <string>
#include <iostream>

#include <ws2tcpip.h>
#include <tcp/socketExceptions.h>

namespace tcp{
    void tcpSocketWrapper::Connect(const char *hostAddress, const char *service) {
        currentState = currentState->Connect(hostAddress, service);
    }

    void tcpSocketWrapper::Write() {}

    void tcpSocketWrapper::Read() {}

    void tcpSocketWrapper::Update() {}

    tcpSocketWrapper::tcpSocketWrapper(){
        WSADATA wsaData;
        const WORD version = MAKEWORD(2,2);

        int result = WSAStartup(version, &wsaData);

        if(result != 0){
            // @TODO Figure out what happened and if it's recoverable. Must be finished before you even think of sharing the code.
        }

        tcpSocket = WSASocketW(addressProtocol, socketType, socketProtocol, socketProtocolInfo, 0, socketFlags);

        if(tcpSocket != INVALID_SOCKET){
            // @TODO Read and react appropriately to socket failure conditions. Do this when considering test cases.
        }

        BOOL ipv6Only = FALSE;
        int ipv6OnlyLen = sizeof(BOOL);

        result = setsockopt(tcpSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &ipv6Only, ipv6OnlyLen);

        // %TODO Figure out if anything can actually go wrong here and address if need be (Or document and let propogate).

        currentState(std::make_unique<initializedState>(initializedState(*this)));
    }

    tcpSocketWrapper::~tcpSocketWrapper() {
        int result;

        if (tcpSocket !=INVALID_SOCKET) {
            result = closesocket(tcpSocket);

            if(result == SOCKET_ERROR){
                std::cout << "Something went wrong when closing the socket." << std::endl;
            }
        }

        result = WSACleanup();
        if(result == SOCKET_ERROR) {
            std::cout << "Something went wrong when closing the socket." << std::endl;
        }
    }

    socketState::socketState(tcpSocketWrapper &socket): socketWrapper(socket){}

    initializedState::initializedState(tcpSocketWrapper &socket): socketState(socket) {}

    std::unique_ptr<socketState> initializedState::Connect(const char *hostAddress, const char *service) {
        // Start the attempt to resolve the address, allow for three attempts to be made before aborting.
        const addrinfo hints = {AI_V4MAPPED, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, nullptr, nullptr, nullptr};
        struct addrinfo *results = nullptr;

        DWORD err;
        int count = 0;

        do{
            err = getaddrinfo(hostAddress, service, &hints, &results);
            if(err != 0) {
                if (count == 3){
                    throw timeoutExcpetion("Failed to resolve the host address after three attempts.");
                } else if(err == WSATRY_AGAIN){
                    count++;
                    continue;
                } else if(err == WSAHOST_NOT_FOUND){
                    throw noSuchAddress(hostAddress, service);
                } else {
                    throw fatalSocketException(err);
                }
            }
        } while (err != 0);

        // Get the next possible point of connection and  create an event to listen for updates on the sockets connect.
        struct addrinfo *next = results->ai_next;
        std::unique_ptr<socketState> state(std::make_unique<connectingState>(connectingState(*this, next)));

        // Attempt to connect to the first option in out getaddrinfo results.
        WSABUF *replyBuffer = nullptr;
        err = WSAConnect(socketWrapper.tcpSocket, results->ai_addr, (int)results->ai_addrlen, nullptr, replyBuffer, nullptr, nullptr);
        if(err != SOCKET_ERROR || WSAGetLastError() != WSAEWOULDBLOCK){
            // @TODO Actually implement errors (this needs to be done before moving to the next level of abstraction)
            // Could be that you conected the socket to a trivial location, could be problem.
        }

        // Get the next possible point of connection and release the memory of the first address attempted.
        results->ai_next = nullptr;
        freeaddrinfo(results);

        // Return the connecting state, passing ownership of all pointers to that state.
        return std::move(state);
    }

    std::unique_ptr<socketState> initializedState::Write() {
        throw invalidStateOperation("Write operations should not be called until the socket is connected.");

    }

    std::unique_ptr<socketState> initializedState::Read() {
        throw invalidStateOperation("Read operations should not be called until the socket is connected.");
    }

    std::unique_ptr<socketState> initializedState::Update() {
        throw invalidStateOperation("Update operation behavior is undefined until socket Connect call is made.");
    }

    connectingState::connectingState(initializedState &curState, addrinfo *next): socketState(curState.socketWrapper) {
        // Create a WSA event to monitor for connection or disconnection events, all error types fatal here.
        connectEvent = WSACreateEvent();

        if(connectEvent == WSA_INVALID_EVENT) {
            DWORD err = WSAGetLastError();
            throw fatalSocketException(err);
        }

        int result = WSAEventSelect(socketWrapper.tcpSocket, connectEvent, FD_CONNECT | FD_CLOSE);
        if (result != 0){
            DWORD err = WSAGetLastError();
            throw fatalSocketException(err);
        }
    }

    std::unique_ptr<socketState> connectingState::Connect(const char *hostAddress, const char *service) {
        throw invalidStateOperation("Can't connect to " + std::string(hostAddress) + "@" + std::string(service)
                                     + ", as the socket is already in the process of connecting.");
    }

    std::unique_ptr<socketState> connectingState::Write() {
        throw invalidStateOperation("Write operations cannot be called until the socket is connected.");
    }

    std::unique_ptr<socketState> connectingState::Read() {
        throw invalidStateOperation("Read operations cannot be called until the socket is connected.");
    }

    // %TODO Check the event log, pass nullptr if no event, respond appropriately otherwise.
    std::unique_ptr<socketState> connectingState::Update() {


    }

    std::unique_ptr<socketState> connectedState::Connect(const char *hostAddress, const char *service) {
        throw invalidStateOperation("Can't connect to " + std::string(hostAddress) + "@" + std::string(service)
                                     + ", as the socket is already connected.");
    }

    std::unique_ptr<socketState> connectedState::Write() {

    }

    std::unique_ptr<socketState> connectedState::Read() {

    }

    std::unique_ptr<socketState> connectedState::Update() {

    }
}
