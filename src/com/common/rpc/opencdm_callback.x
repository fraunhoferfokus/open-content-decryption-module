/*
 * OpenCDM XDR to be used for RPC communication between DRM and CDM platform counterpart
 * Based on EME methods and naming.
 */

struct rpc_cb_message {
    char session_id <>;
    string message <>;
    string destination_url <>;
};

struct rpc_cb_key_status_update {
    char session_id <>;
    string message <>;
};


struct rpc_cb_ready {
    char session_id <>;
};


struct rpc_cb_error {
    char session_id <>;
    int error;
    string error_message <>;
};

program OPEN_CDM_CALLBACK {
    version OPEN_CDM_EME_5 {
    void ON_KEY_MESSAGE(rpc_cb_message) = 1;
    void ON_KEY_READY(rpc_cb_ready) = 2;
    void ON_KEY_ERROR(rpc_cb_error) = 3;
    void ON_KEY_STATUS_UPDATE(rpc_cb_key_status_update) = 4;
    } = 1;
} = 0x66666666;
