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

// Based on file contibuted to the Chromium project
// media/cdm/ppapi/external_clear_key/clear_key_cdm.h
// License notice of original file:

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_H_

#include <string>
#include <vector>
#include "base/basictypes.h"  // needed for media::MediaKeys::Exception
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "media/base/decryptor.h"
#include "media/base/media_export.h"
#include "media/base/media_keys.h"
#include "media/base/decoder_buffer.h"

#include "media/cdm/ppapi/external_open_cdm/browser/chrome/open_cdm_chrome_common.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com_callback_receiver.h"
#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine.h"

#include "media/cdm/ppapi/external_clear_key/ffmpeg_cdm_audio_decoder.h"
#include "media/cdm/ppapi/external_clear_key/ffmpeg_cdm_video_decoder.h"
#include "media/cdm/ppapi/external_clear_key/cdm_video_decoder.h"

#include "map"

namespace media {

class OpenCdm : public OpenCdmInterface,
    public OpenCdmPlatformComCallbackReceiver {
 public:
  OpenCdm(OpenCdmHost* host, const std::string& key_system);
  ~OpenCdm() override;

  // OpenDecryptor MediaKeys implementation
  void CreateSessionAndGenerateRequest(uint32 promise_id,
                                               cdm::SessionType session_type,
                                               const char* init_data_type,
                                               uint32 init_data_type_size,
                                               const uint8* init_data,
                                               uint32 init_data_size) override;
  void LoadSession(uint32 promise_id,
                           cdm::SessionType session_type,
                           const char* web_session_id,
                           uint32_t web_session_id_length) override;

  void CloseSession(uint32 promise_id,
                            const char* web_session_id,
                            uint32_t web_session_id_length) override;

  void UpdateSession(uint32 promise_id,
                             const char* web_session_id,
                             uint32_t web_session_id_length,
                             const uint8* response,
                             uint32 response_size) override;

  void RemoveSession(uint32 promise_id,
                             const char* web_session_id,
                             uint32_t web_session_id_length) override;

  void SetServerCertificate(uint32 promise_id,
                                    const uint8_t* server_certificate_data,
                                    uint32_t server_certificate_data_size)
                                        override;
  void TimerExpired(void* context) override;

  cdm::Status Decrypt(const cdm::InputBuffer& encrypted_buffer,
                              cdm::DecryptedBlock* decrypted_block) override;
  cdm::Status InitializeAudioDecoder(
      const cdm::AudioDecoderConfig& audio_decoder_config) override;
  cdm::Status InitializeVideoDecoder(
      const cdm::VideoDecoderConfig& video_decoder_config) override;
  void DeinitializeDecoder(cdm::StreamType decoder_type) override;
  void ResetDecoder(cdm::StreamType decoder_type) override;
  cdm::Status DecryptAndDecodeFrame(
      const cdm::InputBuffer& encrypted_buffer,
      cdm::VideoFrame* me)
          override;
  cdm::Status DecryptAndDecodeSamples(
      const cdm::InputBuffer& encrypted_buffer, cdm::AudioFrames* audio_frames)
          override;
  void Destroy() override;
  void OnPlatformChallengeResponse(
      const cdm::PlatformChallengeResponse& response) override;

  void OnQueryOutputProtectionStatus(
      cdm::QueryResult result,
      uint32_t link_mask,
      uint32_t output_protection_mask) override;

 private:
  OpenCdmHost* host_;
  OpenCdmMediaengine *media_engine_;
  scoped_ptr<OpenCdmPlatform> platform_;
  std::map<std::string, OpenCdmPlatformSessionId> session_id_map;

  // Todo: remove this. It is from old CDM
  void ReadyCallback(OpenCdmPlatformSessionId platform_session_id) override;

  void ErrorCallback(OpenCdmPlatformSessionId platform_session_id,
                            uint32_t sys_err, std::string err_msg) override;
  void MessageCallback(OpenCdmPlatformSessionId platform_session_id,
                               std::string message,
                               std::string destination_url) override;
  // ContentDecryptionModule callbacks.
  void OnSessionMessage(const std::string& web_session_id,
                        MediaKeys::MessageType message_type,
                        const std::vector<uint8>& message,
                        const GURL& legacy_destination_url);
  void OnSessionKeysChange(const std::string& web_session_id,
                           bool has_additional_usable_key,
                           CdmKeysInfo keys_info);
  void OnSessionClosed(const std::string& web_session_id);

  // Handle the success/failure of a promise. These methods are responsible for
  // calling |host_| to resolve or reject the promise.
  void OnSessionCreated(uint32 promise_id, const std::string& web_session_id);
  void OnSessionLoaded(uint32 promise_id, const std::string& web_session_id);
  void OnPromiseResolved(uint32 promise_id);
  void OnPromiseFailed(uint32 promise_id, MediaKeys::Exception exception_code,
                       uint32 system_code, const std::string& error_message);

 // Prepares next heartbeat message and sets a timer for it.
  void ScheduleNextHeartBeat();

  // Decrypts the |encrypted_buffer| and puts the result in |decrypted_buffer|.
  // Returns cdm::kSuccess if decryption succeeded. The decrypted result is
  // put in |decrypted_buffer|. If |encrypted_buffer| is empty, the
  // |decrypted_buffer| is set to an empty (EOS) buffer.
  // Returns cdm::kNoKey if no decryption key was available. In this case
  // |decrypted_buffer| should be ignored by the caller.
  // Returns cdm::kDecryptError if any decryption error occurred. In this case
  // |decrypted_buffer| should be ignored by the caller.
  cdm::Status DecryptToMediaDecoderBuffer(
      const cdm::InputBuffer& encrypted_buffer,
      scoped_refptr<DecoderBuffer>* decrypted_buffer);

  const std::string key_system_;
  std::string last_session_id_;
  cdm::StreamType last_stream_type_;
  cdm::Status audio_decoder_state_;
  cdm::Status video_decoder_state_;

  virtual OpenCdmPlatformSessionId GetPlatformSessionId(
      const std::string& web_session_id);
  virtual std::string GetChromeSessionId(
      OpenCdmPlatformSessionId platform_session_id);

#if defined(OCDM_USE_FFMPEG_DECODER)
  scoped_ptr<FFmpegCdmAudioDecoder> audio_decoder_;
  scoped_ptr<CdmVideoDecoder> video_decoder_;
#endif  // OPEN_CDM_USE_FFMPEG_DECODER
};

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_H_
