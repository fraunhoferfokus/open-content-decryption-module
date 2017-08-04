/*
 * Copyright (c) 2017 Fraunhofer FOKUS
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_RPC_RPC_CDM_PLATFORM_HANDLER_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_RPC_RPC_CDM_PLATFORM_HANDLER_H_

#include <rpc/rpc.h>
#include <string>

#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_common.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com_callback_receiver.h"
#include "media/cdm/ppapi/external_open_cdm/com/common/rpc/opencdm_callback.h"

namespace media {

class RpcCdmPlatformHandler : public OpenCdmPlatformCom {
 public:
  // NEW EME based interface
  // on errors tear down media keys and media key session objects

  // EME equivalent: new MediaKeys()
  virtual MediaKeysResponse MediaKeys(std::string key_system);

  // EME equivalent: media_keys_.createSession()
  virtual MediaKeysCreateSessionResponse MediaKeysCreateSession(
      const std::string& init_data_type, const uint8_t* init_data,
      int init_data_length);

  // EME equivalent: media_keys_.loadSession()
  virtual MediaKeysLoadSessionResponse MediaKeysLoadSession(
      uint16_t *session_id_val, uint32_t session_id_len);

  // EME equivalent: media_key_session_.update()
  virtual MediaKeySessionUpdateResponse MediaKeySessionUpdate(
      const uint8 *pbKey, uint32 cbKey, uint16_t *session_id_val,
      uint32_t session_id_len);

  // EME equivalent: media_key_session_.release()
  virtual MediaKeySessionReleaseResponse MediaKeySessionRelease(
      uint16_t *session_id_val, uint32_t session_id_len);

  virtual ~RpcCdmPlatformHandler() {
  }
  RpcCdmPlatformHandler(OpenCdmPlatformComCallbackReceiver *callback_receiver);

  // RPC specific behavior
  static void * DelegateRpcInit(void *call_params);
  static void DelegateRpcCallback(struct svc_req *rqstp,
                                  register SVCXPRT *transp);
  static void OnMessage1SvcDelegate(rpc_cb_message *kmm,
                                  struct svc_req *rqstp, RpcCdmPlatformHandler *p_instance);
  static void OnReady1SvcDelegate(rpc_cb_ready *keyready_param,
                                  struct svc_req *rqstp, RpcCdmPlatformHandler *p_instance);
  static void OnError1SvcDelegate(rpc_cb_error *err_param, 
                                  struct svc_req *rqstp, RpcCdmPlatformHandler *p_instance);

 private:
  OpenCdmPlatformComCallbackReceiver *callback_receiver_;
  OpenCdmPlatformSessionId session_id_;
  // TODO(ska): remove, when this gets available in callbacks

  void *RpcInitPrivate(void *thread_parm);
  void RpcCallbackPrivate(struct svc_req *rqstp, register SVCXPRT *transp);
  void OnMessage1Svc(rpc_cb_message *, struct svc_req *);
  void OnReady1Svc(rpc_cb_ready *, struct svc_req *);
  void OnError1Svc(rpc_cb_error *err_param, struct svc_req *rqstp);

  std::string rpc_server_host;
  CLIENT *rpc_client;
  int rpc_prog;
};
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_RPC_RPC_CDM_PLATFORM_HANDLER_H_
