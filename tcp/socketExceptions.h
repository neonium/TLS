//
// Created by ericd on 2019-06-21.
//

#ifndef TLS_SOCKETEXCEPTIONS_H
#define TLS_SOCKETEXCEPTIONS_H

#include <Winsock2.h>

#include <iostream>

// %TODO Move the windows specific code to a windows utility class and compile appropriately.

namespace tcp {
    struct noSuchAddress : public std::exception {
        noSuchAddress( const char* address, const char* service );

        const char* what();

    private:
        const char* address;
        const char* service;
    };

    struct fatalSocketException: public std::runtime_error{
    public:
        explicit fatalSocketException( );
        explicit fatalSocketException( DWORD );
        explicit fatalSocketException( const std::string& );
        explicit fatalSocketException( const char* );
    };

    struct timeoutExcpetion: public std::runtime_error{
    public:
        explicit timeoutExcpetion( );
        explicit timeoutExcpetion( const std::string& );
        explicit timeoutExcpetion( const char* );
    };

    struct invalidStateOperation: public std::runtime_error{
        explicit invalidStateOperation( const std::string& );
        explicit invalidStateOperation( const char* );
    };
}
#endif //TLS_SOCKETEXCEPTIONS_H
