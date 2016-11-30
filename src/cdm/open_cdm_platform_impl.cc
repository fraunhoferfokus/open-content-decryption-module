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

#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_impl.h"
#include "media/cdm/ppapi/external_open_cdm/com/cdm/open_cdm_platform_com_handler_factory.h"
#include "media/cdm/ppapi/cdm_logging.h"

namespace media {

OpenCdmPlatformImpl::OpenCdmPlatformImpl(
    OpenCdmPlatformComCallbackReceiver *callback_receiver)
    : callback_receiver_(callback_receiver) {
  CDM_DLOG() << "new OpenCdmPlatformCdmiImpl instance";
  com_handler_ = OpenCdmPlatformComHandlerFactory::Create(this);
}

MediaKeysResponse OpenCdmPlatformImpl::MediaKeys(std::string key_system) {
  CDM_DLOG() << "OpenCdmPlatformCdmiImpl::MediaKeys";
  MediaKeysResponse response = com_handler_->MediaKeys(key_system);

  return response;
}

MediaKeysCreateSessionResponse OpenCdmPlatformImpl::MediaKeysCreateSession(
    const std::string& init_data_type, const uint8_t* init_data,
    int init_data_length) {
  CDM_DLOG() << "OpenCdmPlatformCdmiImpl::MediaKeysCreateSession";
  MediaKeysCreateSessionResponse response;

  response = com_handler_->MediaKeysCreateSession(init_data_type, init_data,
                                                  init_data_length);

  return response;
}

MediaKeysLoadSessionResponse OpenCdmPlatformImpl::MediaKeysLoadSession(
    char *session_id_val, uint32_t session_id_len) {
  CDM_DLOG() << "OpenCdmPlatformCdmiImpl::MediaKeysLoadSession";
  MediaKeysLoadSessionResponse response;

  response = com_handler_->MediaKeysLoadSession(session_id_val, session_id_len);

  return response;
}

MediaKeySessionUpdateResponse OpenCdmPlatformImpl::MediaKeySessionUpdate(
    const uint8 *pbKey, uint32 cbKey, char *session_id_val,
    uint32_t session_id_len) {
  CDM_DLOG() << "OpenCdmPlatformCdmiImpl::MediaKeySessionUpdate";
  MediaKeySessionUpdateResponse response;

  response = com_handler_->MediaKeySessionUpdate(pbKey, cbKey, session_id_val,
                                                 session_id_len);

  return response;
}

MediaKeySessionReleaseResponse OpenCdmPlatformImpl::MediaKeySessionRelease(
    char *session_id_val, uint32_t session_id_len) {
  CDM_DLOG() << "OpenCdmPlatformCdmiImpl::MediaKeySessionRelease";
  MediaKeySessionReleaseResponse response;

  response = com_handler_->MediaKeySessionRelease(session_id_val,
                                                  session_id_len);

  return response;
}

// OpenCdmComCallbackReceiver inheritance
void OpenCdmPlatformImpl::ErrorCallback(
    OpenCdmPlatformSessionId platform_session_id, uint32_t sys_err,
    std::string err_msg) {
  callback_receiver_->ErrorCallback(platform_session_id, sys_err, err_msg);
}

void OpenCdmPlatformImpl::MessageCallback(
    OpenCdmPlatformSessionId platform_session_id, std::string message,
    std::string destination_url) {
  callback_receiver_->MessageCallback(platform_session_id, message,
                                      destination_url);
}

void OpenCdmPlatformImpl::OnKeyStatusUpdateCallback(
    OpenCdmPlatformSessionId platform_session_id, std::string message
    ) {
  callback_receiver_->OnKeyStatusUpdateCallback(platform_session_id, message);
}
void OpenCdmPlatformImpl::ReadyCallback(
    OpenCdmPlatformSessionId platform_session_id) {
  callback_receiver_->ReadyCallback(platform_session_id);
}

}  // namespace media
