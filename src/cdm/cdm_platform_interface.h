/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/
#ifndef CDM_PLATFORM_INTERFACE_H_
#define CDM_PLATFORM_INTERFACE_H_

#include <stdint.h>
#include <string>

namespace cdm {

  enum PLATFORM_CALL_STATE {
    PLATFORM_CALL_SUCCESS = 0,
    PLATFORM_CALL_FAIL = 1,
    PLATFORM_CALL_NOACTION = 2,
    PLATFORM_CALL_NEEDKEY = 3
  };

  struct PlatformResponse {
    PLATFORM_CALL_STATE platform_response;
    int32_t sys_err;
    uint8_t *pbResponseData;
    uint32_t cbResponseData;
  };

  /*
   * used for encrypting the DRM-decrypted content to ensure a secure
   * transmission between CDMi and CDM
   */
  struct CDMSymmetricKey {
    uint8_t *key;
    uint32_t key_len;
  };

  /*
   * used for secure exchange of the symmetric content key
   */
  struct CDMAsymmetricKeyPair {
    uint8_t *pub_key;
    uint32_t pub_key_len;
  };

  struct PlatformInitPacket {
    PlatformResponse result;
    CDMSymmetricKey media_key;
  };

  struct PlatformKeyMessagePacket {
    PlatformResponse response;
    uint8_t *pbKeyMessage;
    uint32_t cbKeyMessage;
  };

  /**
  * Interface that maps an EME browser implementation to CDMi spec functions
  */
  class ICDMPlatform {
    // TODO(sph): interface currently represents CDMi mappings. Consider
    // changing to EME-specific version mapping.
    // Consider renaming all occurrences of CDM to Cdm
    //, e.g. ICDMPlatform -> ICdmPlatform
   public:
      virtual ~ICDMPlatform() = 0;

      virtual PlatformResponse MediaKeys(std::string keySystem) = 0;

      virtual PlatformResponse MediaKeys_CreateSession(std::string mimeType,
        const uint8_t *pbInitData, uint32_t cbInitData) = 0;
      virtual PlatformResponse MediaKeys_CreateSession(std::string mimeType,
        const uint8_t *pbInitData, uint32_t cbInitData,
        uint8_t *pbCDMData, uint32_t cbCDMData) = 0;
        // TODO(ska): remove as long as CDMData is not part of EME impl

      virtual PlatformResponse MediaKeySession_Run(
        uint16_t *session_id_val, uint32_t session_id_len) = 0;

      virtual PlatformResponse MediaKeySession_Update(
        const uint8_t *pbKey, uint32_t cbKey,
        uint16_t *session_id_val, uint32_t session_id_len) = 0;

      virtual PlatformResponse MediaKeySession_Close(
        uint16_t *session_id_val, uint32_t session_id_len) = 0;
  };
}  // namespace cdm
#endif  // CDM_PLATFORM_INTERFACE_H_
