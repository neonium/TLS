//
// Created by ericd on 2019-06-21.
//

#include "socketExceptions.h"

namespace tcp {
    noSuchAddress::noSuchAddress(const char* addr, const char* ser) : address(addr), service(ser){

    }

    const char* noSuchAddress::what(){
        if(address == nullptr || service == nullptr) {
            return ("Could not find address of server at " + std::string(address) + "@"+ std::string(service)).c_str();
        } else return "Invalid address passed to connect function, connection failed. Closing socket.";
    }

    fatalSocketException::fatalSocketException()
            : std::runtime_error("Unrecoverable socket failure has occurred. Closing the socket."){}
    fatalSocketException::fatalSocketException( const char* what ): std::runtime_error(what){}
    fatalSocketException::fatalSocketException( const std::string& what ): std::runtime_error(what){}

    fatalWSAException::fatalWSAException( const char* what ): std::runtime_error(what){}
    fatalWSAException::fatalWSAException( const std::string& what ): std::runtime_error(what){}

    timeoutExcpetion::timeoutExcpetion()
            : std::runtime_error("Connection timed out."){}
    timeoutExcpetion::timeoutExcpetion( const char* what ): std::runtime_error(what){}
    timeoutExcpetion::timeoutExcpetion( const std::string& what ): std::runtime_error(what){}


    invalidStateOperation::invalidStateOperation( const char* what ): std::runtime_error(what){}
    invalidStateOperation::invalidStateOperation( const std::string& what ): std::runtime_error(what){}

}