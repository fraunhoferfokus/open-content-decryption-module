/*
 * Copyright 2014 Fraunhofer FOKUS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_H_

#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_common.h"

namespace media {

struct MediaEngineSessionResponse : public PlatformResponse {
  uint8_t *pbEncryptedSessionKey;
  uint32_t cbEncryptedSessionKey;
};

struct DecryptResponse : public PlatformResponse {
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
  char *id;
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
class OpenCdmMediaengine {
 public:
  OpenCdmMediaengine(char *session_id_val, uint32_t session_id_len,
                     uint8_t * auth_data_val, uint32_t auth_data_len);
  virtual ~OpenCdmMediaengine() {
  }

  virtual DecryptResponse Decrypt(const uint8_t *pbIv, uint32_t cbIv,
                                  const uint8_t *pbData, uint32_t cbData,
                                  uint8_t *out, uint32_t &out_size) = 0;

 protected:
  OpenCdmMediaengine(char *session_id_val, uint32_t session_id_len);
  OpenCdmMediaengine() {
  }
};

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_H_
