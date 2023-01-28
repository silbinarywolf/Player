#include "net_client.h"
#include <assert.h>
#include <memory>

#define CONNECT_TOKEN_EXPIRY 30
#define CONNECT_TOKEN_TIMEOUT 5

static uint8_t packet_data[NETCODE_MAX_PACKET_SIZE];

static uint8_t private_key[NETCODE_KEY_BYTES] = { 0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea, 
                                                  0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4, 
                                                  0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
                                                  0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1 };

bool Net_Client::Init(double time) {
    if (netcode_init() != NETCODE_OK) {
        //printf("error: failed to initialize netcode.io\n");
        return false;
    }

    netcode_log_level( NETCODE_LOG_LEVEL_INFO );

    printf( "[client]\n" );

    struct netcode_client_config_t client_config;
    netcode_default_client_config( &client_config );
    client = netcode_client_create( "0.0.0.0", &client_config, time );
    if (!client) {
        //printf("error: failed to create client\n");
        return false;
    }
    uint64_t client_id = 0;
    netcode_random_bytes( (uint8_t*) &client_id, 8 );

    uint8_t user_data[NETCODE_USER_DATA_BYTES];
    netcode_random_bytes(user_data, NETCODE_USER_DATA_BYTES);

    uint8_t connect_token[NETCODE_CONNECT_TOKEN_BYTES];

    NETCODE_CONST char * server_address = "127.0.0.1:40000";

    if (netcode_generate_connect_token(
        1, 
        &server_address, 
        &server_address, 
        CONNECT_TOKEN_EXPIRY, 
        CONNECT_TOKEN_TIMEOUT, 
        client_id, 
        PROTOCOL_ID, 
        private_key, 
        user_data, 
        connect_token
    ) != NETCODE_OK) {
        // printf( "error: failed to generate connect token\n" );
        return false;
    }
    netcode_client_connect(client, connect_token);
    for (int i = 0; i < NETCODE_MAX_PACKET_SIZE; ++i) {
        packet_data[i] = (uint8_t) i;
    }
    return true;
}

void Net_Client::Update(double time) {
    assert(client != nullptr);
    netcode_client_update(client, time);

    if (netcode_client_state(client) == NETCODE_CLIENT_STATE_CONNECTED) {
        mode = NetKind::Client;
        netcode_client_send_packet(client, packet_data, NETCODE_MAX_PACKET_SIZE);
    }

    while (1) {
        int packet_bytes;
        uint64_t packet_sequence;
        void* packet = netcode_client_receive_packet( client, &packet_bytes, &packet_sequence );
        if (!packet) {
            break;
        }
        (void) packet_sequence;
        assert(packet_bytes == NETCODE_MAX_PACKET_SIZE);
        assert(memcmp(packet, packet_data, NETCODE_MAX_PACKET_SIZE) == 0);            
        netcode_client_free_packet(client, packet);
    }

    if (netcode_client_state(client) <= NETCODE_CLIENT_STATE_DISCONNECTED ) {
        is_disconnected = true;
        return;
    }
}

void Net_Client::Shutdown() {
   netcode_client_destroy(client);
   netcode_term();
   mode = NetKind::Offline;
   client = nullptr;
}
