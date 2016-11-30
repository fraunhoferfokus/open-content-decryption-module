/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _OPENCDM_XDR_H_RPCGEN
#define _OPENCDM_XDR_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct rpc_request_is_type_supported {
	struct {
		u_int key_system_len;
		char *key_system_val;
	} key_system;
	struct {
		u_int mime_type_len;
		char *mime_type_val;
	} mime_type;
};
typedef struct rpc_request_is_type_supported rpc_request_is_type_supported;

struct rpc_request_mediakeys {
	struct {
		u_int key_system_len;
		char *key_system_val;
	} key_system;
};
typedef struct rpc_request_mediakeys rpc_request_mediakeys;

struct rpc_request_callback_info {
	struct {
		u_int hostname_len;
		char *hostname_val;
	} hostname;
	uint64_t prog_num;
	uint32_t prog_version;
};
typedef struct rpc_request_callback_info rpc_request_callback_info;

struct rpc_request_create_session {
	struct {
		u_int init_data_type_len;
		char *init_data_type_val;
	} init_data_type;
	struct {
		u_int init_data_len;
		uint8_t *init_data_val;
	} init_data;
	rpc_request_callback_info callback_info;
};
typedef struct rpc_request_create_session rpc_request_create_session;

struct rpc_request_load_session {
	struct {
		u_int session_id_len;
		char *session_id_val;
	} session_id;
};
typedef struct rpc_request_load_session rpc_request_load_session;

struct rpc_request_session_update {
	struct {
		u_int session_id_len;
		char *session_id_val;
	} session_id;
	struct {
		u_int key_len;
		uint8_t *key_val;
	} key;
};
typedef struct rpc_request_session_update rpc_request_session_update;

struct rpc_request_session_release {
	struct {
		u_int session_id_len;
		char *session_id_val;
	} session_id;
};
typedef struct rpc_request_session_release rpc_request_session_release;

struct rpc_request_mediaengine_data {
	struct {
		u_int session_id_len;
		char *session_id_val;
	} session_id;
	struct {
		u_int auth_data_len;
		uint8_t *auth_data_val;
	} auth_data;
	int32_t id_exchange_shmem;
	int32_t id_exchange_sem;
};
typedef struct rpc_request_mediaengine_data rpc_request_mediaengine_data;

struct rpc_response_generic {
	int platform_val;
};
typedef struct rpc_response_generic rpc_response_generic;

struct rpc_response_create_session {
	int platform_val;
	struct {
		u_int session_id_len;
		char *session_id_val;
	} session_id;
};
typedef struct rpc_response_create_session rpc_response_create_session;

#define OPEN_CDM 0x61135687
#define OPEN_CDM_EME_5 1

#if defined(__STDC__) || defined(__cplusplus)
#define RPC_OPEN_CDM_IS_TYPE_SUPPORTED 1
extern  rpc_response_generic * rpc_open_cdm_is_type_supported_1(rpc_request_is_type_supported *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_is_type_supported_1_svc(rpc_request_is_type_supported *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAKEYS 2
extern  rpc_response_generic * rpc_open_cdm_mediakeys_1(rpc_request_mediakeys *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_mediakeys_1_svc(rpc_request_mediakeys *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAKEYS_CREATE_SESSION 3
extern  rpc_response_create_session * rpc_open_cdm_mediakeys_create_session_1(rpc_request_create_session *, CLIENT *);
extern  rpc_response_create_session * rpc_open_cdm_mediakeys_create_session_1_svc(rpc_request_create_session *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAKEYS_LOAD_SESSION 4
extern  rpc_response_generic * rpc_open_cdm_mediakeys_load_session_1(rpc_request_load_session *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_mediakeys_load_session_1_svc(rpc_request_load_session *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAKEYSESSION_UPDATE 5
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_update_1(rpc_request_session_update *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_update_1_svc(rpc_request_session_update *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAKEYSESSION_RELEASE 6
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_release_1(rpc_request_session_release *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_release_1_svc(rpc_request_session_release *, struct svc_req *);
#define RPC_OPEN_CDM_MEDIAENGINE 7
extern  rpc_response_generic * rpc_open_cdm_mediaengine_1(rpc_request_mediaengine_data *, CLIENT *);
extern  rpc_response_generic * rpc_open_cdm_mediaengine_1_svc(rpc_request_mediaengine_data *, struct svc_req *);
extern int open_cdm_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define RPC_OPEN_CDM_IS_TYPE_SUPPORTED 1
extern  rpc_response_generic * rpc_open_cdm_is_type_supported_1();
extern  rpc_response_generic * rpc_open_cdm_is_type_supported_1_svc();
#define RPC_OPEN_CDM_MEDIAKEYS 2
extern  rpc_response_generic * rpc_open_cdm_mediakeys_1();
extern  rpc_response_generic * rpc_open_cdm_mediakeys_1_svc();
#define RPC_OPEN_CDM_MEDIAKEYS_CREATE_SESSION 3
extern  rpc_response_create_session * rpc_open_cdm_mediakeys_create_session_1();
extern  rpc_response_create_session * rpc_open_cdm_mediakeys_create_session_1_svc();
#define RPC_OPEN_CDM_MEDIAKEYS_LOAD_SESSION 4
extern  rpc_response_generic * rpc_open_cdm_mediakeys_load_session_1();
extern  rpc_response_generic * rpc_open_cdm_mediakeys_load_session_1_svc();
#define RPC_OPEN_CDM_MEDIAKEYSESSION_UPDATE 5
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_update_1();
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_update_1_svc();
#define RPC_OPEN_CDM_MEDIAKEYSESSION_RELEASE 6
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_release_1();
extern  rpc_response_generic * rpc_open_cdm_mediakeysession_release_1_svc();
#define RPC_OPEN_CDM_MEDIAENGINE 7
extern  rpc_response_generic * rpc_open_cdm_mediaengine_1();
extern  rpc_response_generic * rpc_open_cdm_mediaengine_1_svc();
extern int open_cdm_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_rpc_request_is_type_supported (XDR *, rpc_request_is_type_supported*);
extern  bool_t xdr_rpc_request_mediakeys (XDR *, rpc_request_mediakeys*);
extern  bool_t xdr_rpc_request_callback_info (XDR *, rpc_request_callback_info*);
extern  bool_t xdr_rpc_request_create_session (XDR *, rpc_request_create_session*);
extern  bool_t xdr_rpc_request_load_session (XDR *, rpc_request_load_session*);
extern  bool_t xdr_rpc_request_session_update (XDR *, rpc_request_session_update*);
extern  bool_t xdr_rpc_request_session_release (XDR *, rpc_request_session_release*);
extern  bool_t xdr_rpc_request_mediaengine_data (XDR *, rpc_request_mediaengine_data*);
extern  bool_t xdr_rpc_response_generic (XDR *, rpc_response_generic*);
extern  bool_t xdr_rpc_response_create_session (XDR *, rpc_response_create_session*);

#else /* K&R C */
extern bool_t xdr_rpc_request_is_type_supported ();
extern bool_t xdr_rpc_request_mediakeys ();
extern bool_t xdr_rpc_request_callback_info ();
extern bool_t xdr_rpc_request_create_session ();
extern bool_t xdr_rpc_request_load_session ();
extern bool_t xdr_rpc_request_session_update ();
extern bool_t xdr_rpc_request_session_release ();
extern bool_t xdr_rpc_request_mediaengine_data ();
extern bool_t xdr_rpc_response_generic ();
extern bool_t xdr_rpc_response_create_session ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_OPENCDM_XDR_H_RPCGEN */
