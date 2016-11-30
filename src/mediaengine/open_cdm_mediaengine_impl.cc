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

#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine_impl.h"
#include "media/cdm/ppapi/external_open_cdm/com/mediaengine/rpc/rpc_cdm_mediaengine_handler.h"

#include "media/cdm/ppapi/cdm_logging.h"

namespace media {

OpenCdmMediaengineImpl::OpenCdmMediaengineImpl(char *session_id_val,
                                               uint32_t session_id_len) {
  media_engine_com_ = new RpcCdmMediaengineHandler(session_id_val,
                                                   session_id_len, 0, 0);
  CDM_DLOG() << "Created new media engine impl ";
}

OpenCdmMediaengineImpl::OpenCdmMediaengineImpl(char *session_id_val,
                                               uint32_t session_id_len,
                                               uint8_t *auth_data_val,
                                               uint32_t auth_data_len) {
  // create media engine session
  media_engine_com_ = new RpcCdmMediaengineHandler(session_id_val,
                                                   session_id_len,
                                                   auth_data_val,
                                                   auth_data_len);
}

OpenCdmMediaengineImpl::~OpenCdmMediaengineImpl() {
  CDM_DLOG() << "OpenCdmMediaengineImpl destruct!";
}

DecryptResponse OpenCdmMediaengineImpl::Decrypt(const uint8_t *pbIv,
                                                uint32_t cbIv,
                                                const uint8_t *pbData,
                                                uint32_t cbData, uint8_t *out,
                                                uint32_t &out_size) {
  printf("_------ Decrypt \n");
  CDM_DLOG() << "OpenCdmMediaengineImpl::Decrypt: ";
  DecryptResponse response;

  response = media_engine_com_->Decrypt(pbIv, cbIv, pbData, cbData, out,
                                        out_size);

  return response;
}

}  // namespace media
