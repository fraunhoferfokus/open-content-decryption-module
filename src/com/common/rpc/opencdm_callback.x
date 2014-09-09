/*
 * OpenCDM XDR to be used for RPC communication between DRM and CDM platform counterpart
 * Based on EME methods and naming.
 */

struct rpc_cb_message {
    uint16_t session_id <>;
    string message <>;
    string destination_url <>;
};


struct rpc_cb_ready {
    uint16_t session_id <>;
};


struct rpc_cb_error {
    uint16_t session_id <>;
    int error;
    string error_message <>;
};

program OPEN_CDM_CALLBACK {
    version OPEN_CDM_EME_5 {
    void ON_KEY_MESSAGE(rpc_cb_message) = 1;
    void ON_KEY_READY(rpc_cb_ready) = 2;
    void ON_KEY_ERROR(rpc_cb_error) = 3;
    } = 1;
} = 0x66666666;
