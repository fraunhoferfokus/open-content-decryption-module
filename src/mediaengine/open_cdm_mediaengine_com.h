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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_COM_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_COM_H_

#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine.h"

namespace media {

  class OpenCdmMediaengineCom : public OpenCdmMediaengine {
   public:
    OpenCdmMediaengineCom(char *session_id_val, uint32_t session_id_len,
                          uint8_t *auth_data_val, uint32_t auth_data_len);
    ~OpenCdmMediaengineCom() override {}

   protected:
    OpenCdmMediaengineCom() {}
  };

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_COM_H_
