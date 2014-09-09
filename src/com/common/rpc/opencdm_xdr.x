/*
 * OpenCDM XDR to be used for RPC communication between CDM and DRM platform counterpart
 * Based on EME methods and naming.
 */

/*
 * REQUEST DATA TYPES
 */

struct rpc_request_is_type_supported {
    char key_system <>;
    char mime_type <>;
};

struct rpc_request_mediakeys {
    char key_system <>;
};

struct rpc_request_callback_info {
    char hostname <>;
    uint64_t prog_num;
    uint32_t prog_version;
};

struct rpc_request_create_session {
    char init_data_type <>;
    uint8_t init_data <>;
    rpc_request_callback_info callback_info;
};

struct rpc_request_load_session {
    uint16_t session_id <>;
};

struct rpc_request_session_update {
    uint16_t session_id <>;
    uint8_t key <>;
};

struct rpc_request_session_release {
    uint16_t session_id <>;
};

struct rpc_request_mediaengine_data {
    uint16_t session_id <>;
    uint8_t auth_data <>;
    int32_t id_exchange_shmem;
    int32_t id_exchange_sem;
};

/*
 * RESPONSE DATA TYPES
 */

struct rpc_response_generic {
    int platform_val;
};

struct rpc_response_create_session {
    int platform_val;
    uint16_t session_id <>;
};

program OPEN_CDM {
    version OPEN_CDM_EME_5 {
    rpc_response_generic RPC_OPEN_CDM_IS_TYPE_SUPPORTED(rpc_request_is_type_supported) = 1;
    rpc_response_generic RPC_OPEN_CDM_MEDIAKEYS(rpc_request_mediakeys) = 2;
    rpc_response_create_session RPC_OPEN_CDM_MEDIAKEYS_CREATE_SESSION(rpc_request_create_session) = 3;
    rpc_response_generic RPC_OPEN_CDM_MEDIAKEYS_LOAD_SESSION(rpc_request_load_session) = 4;
    rpc_response_generic RPC_OPEN_CDM_MEDIAKEYSESSION_UPDATE(rpc_request_session_update) = 5;
    rpc_response_generic RPC_OPEN_CDM_MEDIAKEYSESSION_RELEASE(rpc_request_session_release) = 6;
    rpc_response_generic RPC_OPEN_CDM_MEDIAENGINE(rpc_request_mediaengine_data) = 7;
    } = 1;
} = 0x61135687; /* FAMEFHG */
