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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_CALLBACK_RECEIVER_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_CALLBACK_RECEIVER_H_

#include <string>

namespace media {

/*
 * This interface serves as receiver interface for callbacks that are
 * necessary to be handled by the platform caller.
 */
class OpenCdmPlatformComCallbackReceiver {
 public:
  // according to EME

  virtual void ErrorCallback(OpenCdmPlatformSessionId platform_session_id,
                             uint32_t sys_err, std::string err_msg) = 0;
  virtual void MessageCallback(OpenCdmPlatformSessionId platform_session_id,
                               std::string message,
                               std::string destination_url) = 0;
  virtual void OnKeyStatusUpdateCallback(OpenCdmPlatformSessionId platform_session_id,
                               std::string message) = 0;
  // TODO(ska): message should be uint8_t*
  virtual void ReadyCallback(OpenCdmPlatformSessionId platform_session_id) = 0;

  virtual ~OpenCdmPlatformComCallbackReceiver() {
  }
 protected:
  OpenCdmPlatformComCallbackReceiver() {
  }
};
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_CALLBACK_RECEIVER_H_
