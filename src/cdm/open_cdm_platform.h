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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_H_

#include <string>
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_common.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com_callback_receiver.h"

namespace media {

class OpenCdmPlatform {
 public:
  // NEW EME based interface
  // on errors tear down media keys and media key session objects

  // EME equivalent: new MediaKeys()
  virtual MediaKeysResponse MediaKeys(std::string key_system) = 0;

  // EME equivalent: media_keys_.createSession()
  virtual MediaKeysCreateSessionResponse MediaKeysCreateSession(
      const std::string& init_data_type, const uint8_t* init_data,
      int init_data_length) = 0;

  // EME equivalent: media_keys_.loadSession()
  virtual MediaKeysLoadSessionResponse MediaKeysLoadSession(
      char *session_id_val, uint32_t session_id_len) = 0;

  // EME equivalent: media_key_session_.update()
  virtual MediaKeySessionUpdateResponse MediaKeySessionUpdate(
      const uint8 *pbKey, uint32 cbKey, char *session_id_val,
      uint32_t session_id_len) = 0;

  // EME equivalent: media_key_session_.release()
  virtual MediaKeySessionReleaseResponse MediaKeySessionRelease(
      char *session_id_val, uint32_t session_id_len) = 0;


  virtual ~OpenCdmPlatform() {
  }
  OpenCdmPlatform(OpenCdmPlatformComCallbackReceiver *callback_receiver_);

 protected:
  OpenCdmPlatform() {
  }
};
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_H_
