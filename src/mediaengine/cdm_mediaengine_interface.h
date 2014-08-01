/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/

#ifndef CDM_MEDIAENGINE_INTERFACE_H_
#define CDM_MEDIAENGINE_INTERFACE_H_

#include "../cdm/cdm_platform_interface.h"

namespace cdm {

  struct MediaEngineSessionResponse {
    PLATFORM_CALL_STATE platform_response;
    int32_t sys_err;
    uint8_t *pbEncryptedSessionKey;
    uint32_t cbEncryptedSessionKey;
  };

  struct DecryptResponse {
    PLATFORM_CALL_STATE platform_response;
    int32_t sys_err;
    uint8_t *pbResponseData;
    uint32_t cbResponseData;
  };

  /*
   * used for encrypting the DRM-decrypted content to ensure a secure
   * transmission between CDMi and CDM mediaengine
   */
  struct MediaEngineSessionKey {
    uint8_t *key;
    uint32_t keyLen;
  };

  struct MediaEngineSessionId {
    uint16_t *id;
    uint32_t idLen;
  };

  /* Extended EME: // REVIEW(ska): need better explanation for this
   * MediaKeys
   *    1*|
   *      |
   *    N*|
   * MediaKeySession 1*----1* MediaEngineSession
   */

  /* MediaEngineSession is interface only */
  class IMediaEngineSession {
   public:
    virtual ~IMediaEngineSession() = 0;  // TODO(ska) destructor = 0?

    virtual DecryptResponse Decrypt(
      uint8_t *pbIv, uint32_t cbIv,
      uint8_t *pbData, uint32_t cbData,
      uint8_t *out, uint32_t &out_size) = 0;
  };

}  // namespace cdm

#endif  // CDM_MEDIAENGINE_INTERFACE_H_
