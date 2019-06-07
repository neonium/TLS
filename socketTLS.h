//
// Created by Eric Vachon on 2019-06-03.
//

#ifndef TLS_SOCKETTLS_H
#define TLS_SOCKETTLS_H

typedef unsigned char uint8;
typedef uint8 uint16[2];
typedef uint8 uint24[3];
typedef uint8 uint32[4];

enum HandshakeType : uint8{
    client_hello = 1,
    server_hello = 2,
    new_session_ticket = 4,
    end_of_early_data = 5,
    encrypted_extensions = 8,
    certificate = 11,
    certificate_request = 13,
    certificate_verify = 15,
    finished = 20,
    key_update = 24,
    message_hash = 254
} ;

struct Handshake {
    HandshakeType msg_type; /* handshake type */
    unsigned short int length[3]; /* remaining bytes in message */
    switch (Handshake.msg_type) {
        case client_hello: ClientHello;
        case server_hello: ServerHello;
        case end_of_early_data: EndOfEarlyData;
        case encrypted_extensions: EncryptedExtensions;
        case certificate_request: CertificateRequest;
        case certificate: Certificate;
        case certificate_verify: CertificateVerify;
        case finished: Finished;
        case new_session_ticket: NewSessionTicket;
        case key_update: KeyUpdate;
    };
};

class socketTLS {

};


#endif //TLS_SOCKETTLS_H
