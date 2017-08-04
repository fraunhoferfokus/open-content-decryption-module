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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_IMPL_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_IMPL_H_

#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine.h"
#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine_com.h"
#include <rpc/rpc.h>
#include <string>
#include "media/cdm/ppapi/external_open_cdm/com/common/shmemsem/shmemsem_helper.h"

namespace media {

/**
 * MediaEngineSession serves as an interface between a platform's mediaengine
 * and the CDMi. It is established as soon as EME signals 'keyAdded'.
 * RPC and shared memory are used for communication.
 * Secure channel decryption is not included.
 */
class OpenCdmMediaengineImpl : public OpenCdmMediaengine {
 public:
  OpenCdmMediaengineImpl(uint16_t *session_id_val, uint32_t session_id_len);
  OpenCdmMediaengineImpl(uint16_t *session_id_val, uint32_t session_id_len,
                         uint8_t *auth_data_val, uint32_t auth_data_len);

  // synchronous decryption
  virtual DecryptResponse Decrypt(const uint8_t *pbIv, uint32_t cbIv,
                                  const uint8_t *pbData, uint32_t cbData,
                                  uint8_t *out, uint32_t &out_size);

  virtual ~OpenCdmMediaengineImpl();
 private:
  OpenCdmMediaengineCom *media_engine_com_;
};

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_IMPL_H_
