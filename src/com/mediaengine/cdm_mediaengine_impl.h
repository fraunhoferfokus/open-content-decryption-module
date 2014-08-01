/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/
#ifndef CDM_MEDIAENGINE_IMPL_H_
#define CDM_MEDIAENGINE_IMPL_H_

#include "../../mediaengine/cdm_mediaengine_interface.h"
#include <rpc/rpc.h>
#include <string>
#include "../common/shmemsem/shmemsem_helper.h"


namespace cdm {

/**
 * MediaEngineSession serves as an interface between a platform's mediaengine
 * and the CDMi. It is established as soon as EME signals 'keyAdded'.
 * RPC and shared memory are used for communication.
 * Secure channel decryption is not included.
 */
  class MediaEngineSession : public IMediaEngineSession {
   public:
      MediaEngineSession(uint16_t *session_id_val, uint32_t session_id_len);

      // synchronous decryption
      virtual DecryptResponse Decrypt(uint8_t *pbIv, uint32_t cbIv,
        uint8_t *pbData, uint32_t cbData, uint8_t *out, uint32_t &out_size);

      ~MediaEngineSession();

   private:
      void SessionDecrypt(uint8_t *pbData, uint32_t cbData);
      // TODO(ska): MediaKeySession seems to be necessary to be handled in the
      // media engine on destruction if the JS application is not able to
      // do it by its own, e.g. due to page reload etc
      void MediaKeySession_Close();

      MediaEngineSessionKey sessionKeyPlaintext;
      MediaEngineSessionId sessionId;

      static std::string rpcServer;
      CLIENT *rpcClient;

      // shared memory
      shmem_info *shMemInfo;
      int idXchngShMem;
      int idXchngSem;
  };


}  // namespace cdm

#endif  // CDM_MEDIAENGINE_IMPL_H_
