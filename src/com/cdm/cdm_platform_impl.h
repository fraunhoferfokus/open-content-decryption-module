/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/
#ifndef CDM_PLATFORM_IMPL_H_
#define CDM_PLATFORM_IMPL_H_

#include "../../cdm/cdm_platform_interface.h"

#include <rpc/rpc.h>
#include <string>

namespace cdm {

  /**
  * CDMPlatform uses RPC for EME flow communication with CDMi
  */
  class CDMPlatform : public ICDMPlatform {
   public:
      // TODO(ska): should not be public, needs static?
      // probably should not even be in this class
      // i think we need some rpc specific communication class
      // that handles this stuff, so we can also change to other
      // mechanisms easily
    static void Platform_StartRpcService();

    virtual PlatformResponse MediaKeys(std::string keySystem);

    virtual PlatformResponse MediaKeys_CreateSession(std::string mimeType,
      const uint8_t *pbInitData, uint32_t cbInitData);
    virtual PlatformResponse MediaKeys_CreateSession(std::string mimeType,
      const uint8_t *pbInitData, uint32_t cbInitData,
      // TODO(ska): why const pbInitData?
      uint8_t *pbCDMData, uint32_t cbCDMData);

    virtual PlatformResponse MediaKeySession_Run(
      uint16_t *session_id_val, uint32_t session_id_len);

    virtual PlatformResponse MediaKeySession_Update(
      const uint8_t *pbKey, uint32_t cbKey,  // TODO(ska): why const pbKey?
      uint16_t *session_id_val, uint32_t session_id_len);

    virtual PlatformResponse MediaKeySession_Close(
      uint16_t *session_id_val, uint32_t session_id_len);

    uint32_t SessionIdLength();  // TODO(ska): still needed?
    uint16_t* SessionIdValue();

    CDMPlatform();
    ~CDMPlatform();

   private:
    CLIENT *rpcClient;
    static std::string rpcServer;  // TODO(ska): should not be static

    // TODO(sph): may need array/vector for holding
    // multiple session key identifiers
    uint32_t m_session_id_len;
    uint16_t *m_session_id_val;

  };
}  // namespace cdm

#endif  // CDM_PLATFORM_IMPL_H_
