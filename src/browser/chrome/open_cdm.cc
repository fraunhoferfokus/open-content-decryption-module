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
// media/cdm/ppapi/external_clear_key/clear_key_cdm.cc
// License notice of original file:

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cdm/ppapi/external_open_cdm/browser/chrome/open_cdm.h"

// Include FFmpeg avformat.h for av_register_all().
extern "C" {
// Temporarily disable possible loss of data warning.
  MSVC_PUSH_DISABLE_WARNING(4244);
#include <libavformat/avformat.h>
  MSVC_POP_WARNING();
}  // extern "C"

#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine_factory.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_factory.h"
#include "media/cdm/ppapi/external_open_cdm/common/open_cdm_common.h"

#include "media/cdm/ppapi/cdm_logging.h"
#include "base/bind.h"
#include "media/base/cdm_promise.h"
#include "media/base/decoder_buffer.h"
#include "media/base/decrypt_config.h"
#include "base/strings/string_number_conversions.h"

#if defined(OCDM_USE_FFMPEG_DECODER)

#include "base/at_exit.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "media/base/media.h"



// TODO(tomfinegan): When COMPONENT_BUILD is not defined an AtExitManager must
// exist before the call to InitializeFFmpegLibraries(). This should no longer
// be required after http://crbug.com/91970 because we'll be able to get rid of
// InitializeFFmpegLibraries().
#if !defined COMPONENT_BUILD
static base::AtExitManager g_at_exit_manager;
#endif  // COMPONENT_BUILD

// TODO(tomfinegan): InitializeFFmpegLibraries() and |g_cdm_module_initialized|
// are required for running in the sandbox, and should no longer be required
// after http://crbug.com/91970 is fixed.
static bool InitializeFFmpegLibraries() {
  base::FilePath file_path;
  CHECK(PathService::Get(base::DIR_MODULE, &file_path));
  CHECK(media::InitializeMediaLibrary(file_path));
  return true;
}

static bool g_ffmpeg_lib_initialized = InitializeFFmpegLibraries();

#endif  // OCDM_USE_FFMPEG_DECODER

// Copies |input_buffer| into a media::DecoderBuffer. If the |input_buffer| is
// empty, an empty (end-of-stream) media::DecoderBuffer is returned.
static scoped_refptr<media::DecoderBuffer> CopyDecoderBufferFrom(
    const cdm::InputBuffer& input_buffer) {
  if (!input_buffer.data) {
    DCHECK(!input_buffer.data_size);
    return media::DecoderBuffer::CreateEOSBuffer();
  }

  // TODO(xhwang): Get rid of this copy.
  scoped_refptr<media::DecoderBuffer> output_buffer =
      media::DecoderBuffer::CopyFrom(input_buffer.data, input_buffer.data_size);

  std::vector<media::SubsampleEntry> subsamples;
  for (uint32_t i = 0; i < input_buffer.num_subsamples; ++i) {
    media::SubsampleEntry subsample;
    subsample.clear_bytes = input_buffer.subsamples[i].clear_bytes;
    subsample.cypher_bytes = input_buffer.subsamples[i].cipher_bytes;
    subsamples.push_back(subsample);
  }

  DCHECK_EQ(input_buffer.data_offset, 0u);
  scoped_ptr<media::DecryptConfig> decrypt_config(
      new media::DecryptConfig(
          std::string(reinterpret_cast<const char*>(input_buffer.key_id),
                      input_buffer.key_id_size),
          std::string(reinterpret_cast<const char*>(input_buffer.iv),
                      input_buffer.iv_size),
          subsamples));

  output_buffer->set_decrypt_config(decrypt_config.Pass());
  output_buffer->set_timestamp(
      base::TimeDelta::FromMicroseconds(input_buffer.timestamp));

  return output_buffer;
}

// OVERRIDE from content_decryption_modue.h
void INITIALIZE_CDM_MODULE() {
#if defined(OCDM_USE_FFMPEG_DECODER)
  av_register_all();
#endif  // OCDM_USE_FFMPEG_DECODER
}

void DeinitializeCdmModule() {
}

static cdm::Error ConvertException(media::MediaKeys::Exception exception_code) {
  switch (exception_code) {
    case media::MediaKeys::NOT_SUPPORTED_ERROR:
      return cdm::kNotSupportedError;
    case media::MediaKeys::INVALID_STATE_ERROR:
      return cdm::kInvalidStateError;
    case media::MediaKeys::INVALID_ACCESS_ERROR:
      return cdm::kInvalidAccessError;
    case media::MediaKeys::QUOTA_EXCEEDED_ERROR:
      return cdm::kQuotaExceededError;
    case media::MediaKeys::UNKNOWN_ERROR:
      return cdm::kUnknownError;
    case media::MediaKeys::CLIENT_ERROR:
      return cdm::kClientError;
    case media::MediaKeys::OUTPUT_ERROR:
      return cdm::kOutputError;
  }
  NOTIMPLEMENTED();
  return cdm::kUnknownError;
}

void* CreateCdmInstance(int cdm_interface_version, const char* key_system,
                        uint32_t key_system_size,
                        GetCdmHostFunc get_cdm_host_func, void* user_data) {
  std::string key_system_string(key_system, key_system_size);

  if (key_system_string != media::kExternalOpenCdmKeySystem) {
    return NULL;
  }

  if (cdm_interface_version != media::OpenCdmInterface::kVersion)
    return NULL;

  media::OpenCdmHost* host = static_cast<media::OpenCdmHost*>(get_cdm_host_func(
      media::OpenCdmHost::kVersion, user_data));
  if (!host)
    return NULL;

  return new media::OpenCdm(host, key_system_string);
}

const char* GetCdmVersion() {
  return media::kOpenCdmVersion;
}

namespace media {

static uint32_t next_web_session_id_ = 1;

OpenCdm::OpenCdm(OpenCdmHost* host, const std::string& key_system)
    : host_(host),
      media_engine_(NULL),
      key_system_(key_system) {

  CDM_DLOG() << "OpenDecryptor construct: key_system";
  audio_decoder_state_ = cdm::kDeferredInitialization;
  video_decoder_state_ = cdm::kDeferredInitialization;
  last_session_id_ = base::UintToString(media::MediaKeys::kInvalidSessionId);

  platform_ = scoped_ptr<OpenCdmPlatform>(
      OpenCdmPlatformInterfaceFactory::Create(this));

  platform_->MediaKeys(key_system);
  CDM_DLOG() << "OpenCdm: created" << "\n";
}

OpenCdm::~OpenCdm() {
}

void OpenCdm::MessageCallback(OpenCdmPlatformSessionId platform_session_id,
                              std::string message,
                              std::string destination_url) {
  CDM_DLOG() << "OpenCdm::MessageCallback";

  std::vector<uint8> send_message;
  send_message.assign(message.c_str(), message.c_str() + message.size());

  CDM_DLOG() << "message: " << message;
  CDM_DLOG() << "la_url: " << destination_url;
  std::string web_session_id = GetChromeSessionId(platform_session_id);
  CDM_DLOG() << "web_session_id: " << web_session_id;
  OnSessionMessage(web_session_id, send_message, GURL(destination_url));
  CDM_DLOG() << "send OnSessionMessage for: " << web_session_id;
}

void OpenCdm::ReadyCallback(OpenCdmPlatformSessionId platform_session_id) {
  CDM_DLOG() << "OpenCdm::ReadyCallback";

  std::string web_session_id = GetChromeSessionId(platform_session_id);
  CDM_DLOG() << "web_session_id: " << web_session_id;
  OnSessionReady(web_session_id);
  CDM_DLOG() << "session_ready_cb_ fired";
}

void OpenCdm::ErrorCallback(OpenCdmPlatformSessionId platform_session_id,
                            uint32_t sys_err, std::string err_msg) {
  CDM_DLOG() << "OpenCdm::ErrorCallback";
  std::string web_session_id = GetChromeSessionId(platform_session_id);
  CDM_DLOG() << "web_session_id: " << web_session_id;
  OnSessionError(web_session_id, media::MediaKeys::UNKNOWN_ERROR, sys_err,
                 err_msg);
  CDM_DLOG() << "session_error_cb_ fired";
}

void OpenCdm::CreateSession(uint32 promise_id, const char* init_data_type,
                            uint32 init_data_type_size, const uint8* init_data,
                            uint32 init_data_size,
                            cdm::SessionType session_type) {
  // TODO(ska): handle session_type
  CDM_DLOG() << " OpenCdm::CreateSession promise_id: " << promise_id
             << " - session_type: " << session_type;

  scoped_ptr<media::NewSessionCdmPromise> promise(
      new media::NewSessionCdmPromise(
          base::Bind(&OpenCdm::OnSessionCreated, base::Unretained(this),
                     promise_id),
          base::Bind(&OpenCdm::OnPromiseFailed, base::Unretained(this),
                     promise_id)));

  CDM_DLOG() << "OpenCdmDecryptor::CreateSession";
  uint32_t renderer_session_id = next_web_session_id_++;
  std::string web_session_id = base::UintToString(renderer_session_id);

  MediaKeysCreateSessionResponse response = platform_->MediaKeysCreateSession(
      init_data_type, init_data, init_data_size);
  if (response.platform_response == PLATFORM_CALL_SUCCESS) {
    this->session_id_map[web_session_id] = response.session_id;
    std::string debug_web_session_id = GetChromeSessionId(response.session_id);
    CDM_DLOG() << "New Session promise resolved, last_session_id_: "
               << web_session_id;
    promise->resolve(web_session_id);
  } else {
    CDM_DLOG() << "reject create session promise";
    promise->reject(media::MediaKeys::INVALID_STATE_ERROR,
                    response.platform_response,
                    "MediaKeySession could not be created.");
    return;
  }
}

void OpenCdm::LoadSession(uint32 promise_id, const char* web_session_id,
                          uint32_t web_session_id_length) {
  CDM_DLOG() << __FUNCTION__;
  NOTIMPLEMENTED();
}

void OpenCdm::UpdateSession(uint32 promise_id, const char* web_session_id,
                            uint32_t web_session_id_size, const uint8* response,
                            uint32 response_size) {
  CDM_DLOG() << " OpenCdm::UpdateSession for " << web_session_id;

  std::string web_session_str(web_session_id, web_session_id_size);

  scoped_ptr<media::SimpleCdmPromise> promise(
      new media::SimpleCdmPromise(
          base::Bind(&OpenCdm::OnSessionUpdated, base::Unretained(this),
                     promise_id, web_session_id),
          base::Bind(&OpenCdm::OnPromiseFailed, base::Unretained(this),
                     promise_id)));

  CDM_DLOG() << " OpenCdmDecryptor::UpdateSession ";

  CHECK(response);
  CHECK_GT(response_size, 0);

  CDM_DLOG() << "UpdateSession: response_length: " << response_size;
  CDM_DLOG() << "UpdateSession: web_session_id: " << web_session_id;

  std::string key_string(reinterpret_cast<const char*>(response),
                         response_size);

  if (session_id_map.find(web_session_id) != session_id_map.end()) {
    promise->resolve();
    platform_->MediaKeySessionUpdate(
        response, response_size, session_id_map[web_session_id].session_id,
        session_id_map[web_session_id].session_id_len);
  } else {
    promise->reject(media::MediaKeys::INVALID_ACCESS_ERROR, 0,
                    "Session does not exist.");
    return;
  }

//  {
//    base::AutoLock auto_lock(new_key_cb_lock_);
//
//    if (!new_audio_key_cb_.is_null())
//      new_audio_key_cb_.Run();
//
//    if (!new_video_key_cb_.is_null())
//      new_video_key_cb_.Run();
//  }
}

void OpenCdm::ReleaseSession(uint32 promise_id, const char* web_session_id,
                             uint32_t web_session_id_size) {
  CDM_DLOG() << " OpenCdm::ReleaseSession ";

  scoped_ptr<media::SimpleCdmPromise> promise(
      new media::SimpleCdmPromise(
          base::Bind(&OpenCdm::OnSessionReleased, base::Unretained(this),
                     promise_id, web_session_id),
          base::Bind(&OpenCdm::OnPromiseFailed, base::Unretained(this),
                     promise_id)));

  if (session_id_map.find(web_session_id) != session_id_map.end()) {
    platform_->MediaKeySessionRelease(
        session_id_map[web_session_id].session_id,
        session_id_map[web_session_id].session_id_len);
    last_session_id_ = "";
    std::map<std::string, OpenCdmPlatformSessionId>::iterator iterator =
        session_id_map.find(web_session_id);
    session_id_map.erase(iterator);
    promise->resolve();
  } else {
    promise->reject(media::MediaKeys::INVALID_ACCESS_ERROR, 0,
                    "Session does not exist.");
    return;
  }
}

void OpenCdm::SetServerCertificate(uint32 promise_id,
                                   const uint8_t* server_certificate_data,
                                   uint32_t server_certificate_data_size) {
  CDM_DLOG() << " OpenCdm::SetServerCertificate ";
  NOTIMPLEMENTED();
}

void OpenCdm::TimerExpired(void* context) {
  CDM_DLOG() << " OpenCdm::SetServerCertificate ";
  NOTIMPLEMENTED();
}

enum ClearBytesBufferSel {
  kSrcContainsClearBytes,
  kDstContainsClearBytes
};

static void CopySubsamples(const std::vector<SubsampleEntry>& subsamples,
                           const ClearBytesBufferSel sel, const uint8* src,
                           uint8* dst) {
  for (size_t i = 0; i < subsamples.size(); i++) {
    const SubsampleEntry& subsample = subsamples[i];
    if (sel == kSrcContainsClearBytes) {
      src += subsample.clear_bytes;
    } else {
      dst += subsample.clear_bytes;
    }
    memcpy(dst, src, subsample.cypher_bytes);
    src += subsample.cypher_bytes;
    dst += subsample.cypher_bytes;
  }
}

cdm::Status OpenCdm::Decrypt(const cdm::InputBuffer& encrypted_buffer,
                             cdm::DecryptedBlock* decrypted_block) {
  CDM_DLOG() << "OpenCdm::Decrypt()";

  DCHECK(encrypted_buffer.data);

  scoped_refptr<media::DecoderBuffer> buffer;
  cdm::Status status = DecryptToMediaDecoderBuffer(encrypted_buffer, &buffer);

  if (status != cdm::kSuccess)
    return status;

  DCHECK(buffer->data());
  decrypted_block->SetDecryptedBuffer(host_->Allocate(buffer->data_size()));
  memcpy(reinterpret_cast<void*>(decrypted_block->DecryptedBuffer()->Data()),
         buffer->data(), buffer->data_size());
  decrypted_block->DecryptedBuffer()->SetSize(buffer->data_size());
  decrypted_block->SetTimestamp(buffer->timestamp().InMicroseconds());

  return cdm::kSuccess;
}

cdm::Status OpenCdm::InitializeAudioDecoder(
    const cdm::AudioDecoderConfig& audio_decoder_config) {
  CDM_DLOG() << "OpenCdm::InitializeAudioDecoder()";
  last_stream_type_ = cdm::kStreamTypeAudio;

#if defined(OCDM_USE_FFMPEG_DECODER)
  if (!audio_decoder_)
  audio_decoder_.reset(new media::FFmpegCdmAudioDecoder(host_));
  // TODO(ska): consider using factory method to instantiate
  // AudioDecoder (see VideoDecoder)

  if (!audio_decoder_->Initialize(audio_decoder_config))
  return cdm::kSessionError;
  CDM_DLOG() << "AudioDecoder initialized";

  return audio_decoder_state_;
#else
  NOTIMPLEMENTED();
  return cdm::kSessionError;
#endif  // OCDM_USE_FFMPEG_DECODER
}

cdm::Status OpenCdm::InitializeVideoDecoder(
    const cdm::VideoDecoderConfig& video_decoder_config) {
  CDM_DLOG() << "OpenCdm::InitializeVideoDecoder()";
  last_stream_type_ = cdm::kStreamTypeVideo;

#if defined(OCDM_USE_FFMPEG_DECODER)
  if (video_decoder_ && video_decoder_->is_initialized()) {
    DCHECK(!video_decoder_->is_initialized());
    return cdm::kSessionError;
  }
  // Any uninitialized decoder will be replaced.
  video_decoder_ = CreateVideoDecoder(host_, video_decoder_config);
  if (!video_decoder_)
  return cdm::kSessionError;
  CDM_DLOG() << "VideoDecoder initialized";

  return video_decoder_state_;
#else
  NOTIMPLEMENTED();
  return cdm::kSessionError;
#endif  // OCDM_USE_FFMPEG_DECODER
}

void OpenCdm::ResetDecoder(cdm::StreamType decoder_type) {
  CDM_DLOG() << "OpenCdm::ResetDecoder()";
#if defined(OCDM_USE_FFMPEG_DECODER)
  switch (decoder_type) {
    case cdm::kStreamTypeVideo:
    video_decoder_->Reset();
    break;
    case cdm::kStreamTypeAudio:
    audio_decoder_->Reset();
    break;
    default:
    NOTREACHED() << "ResetDecoder(): invalid cdm::StreamType";
  }
#endif  // OCDM_USE_FFMPEG_DECODER
}

void OpenCdm::DeinitializeDecoder(cdm::StreamType decoder_type) {
  CDM_DLOG() << "OpenCdm::DeinitializeDecoder()";
  switch (decoder_type) {
#if defined(OCDM_USE_FFMPEG_DECODER)
    case cdm::kStreamTypeVideo:
    video_decoder_->Deinitialize();
    break;
    case cdm::kStreamTypeAudio:
    audio_decoder_->Deinitialize();
#endif
    break;
  default:
    NOTREACHED() << "DeinitializeDecoder(): invalid cdm::StreamType";
  }
}

cdm::Status OpenCdm::DecryptAndDecodeFrame(
    const cdm::InputBuffer& encrypted_buffer, cdm::VideoFrame* decoded_frame) {
  CDM_DLOG() << "OpenCdm::DecryptAndDecodeFrame()";
#if defined(OCDM_USE_FFMPEG_DECODER)
  scoped_refptr<media::DecoderBuffer> buffer;
  cdm::Status status = DecryptToMediaDecoderBuffer(encrypted_buffer, &buffer);

  if (status != cdm::kSuccess)
  return status;

  const uint8_t* data = NULL;
  int32_t size = 0;
  int64_t timestamp = 0;
  if (!buffer->end_of_stream()) {
    data = buffer->data();
    size = buffer->data_size();
    timestamp = encrypted_buffer.timestamp;
  }

  return video_decoder_->DecodeFrame(data, size, timestamp, decoded_frame);
#else
  return cdm::kDecryptError;
#endif
}

cdm::Status OpenCdm::DecryptAndDecodeSamples(
    const cdm::InputBuffer& encrypted_buffer, cdm::AudioFrames* audio_frames) {
  CDM_DLOG() << "OpenCdm::DecryptAndDecodeSamples()";

  scoped_refptr<media::DecoderBuffer> buffer;
  cdm::Status status = DecryptToMediaDecoderBuffer(encrypted_buffer, &buffer);

  if (status != cdm::kSuccess)
    return status;

#if defined(OCDM_USE_FFMPEG_DECODER)
  const uint8_t* data = NULL;
  int32_t size = 0;
  int64_t timestamp = 0;
  if (!buffer->end_of_stream()) {
    data = buffer->data();
    size = buffer->data_size();
    timestamp = encrypted_buffer.timestamp;
  }

  return audio_decoder_->DecodeBuffer(data, size, timestamp, audio_frames);
#else
  return cdm::kDecryptError;
#endif  //  OCDM_USE_FFMPEG_DECODER
}

void OpenCdm::Destroy() {
  CDM_DLOG() << "OpenCdm::Destroy()" << "\n";
  delete this;
}

OpenCdmPlatformSessionId OpenCdm::GetPlatformSessionId(
    const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::GetPlatformSessionId" << "\n";

  OpenCdmPlatformSessionId session_id;

  if (session_id_map.find(web_session_id) != session_id_map.end()) {
    session_id = session_id_map[web_session_id];
  }

  return session_id;
}

std::string OpenCdm::GetChromeSessionId(
    OpenCdmPlatformSessionId platform_session_id) {
  CDM_DLOG() << "OpenCdm::GetChromeSessionId";
  CDM_DLOG() << "OpenCdm::GetChromeSessionId, sid.len: "
             << platform_session_id.session_id_len;
  CDM_DLOG() << "OpenCdm::GetChromeSessionId, sid[0]: "
             << platform_session_id.session_id[0];
  CDM_DLOG() << "OpenCdm::GetChromeSessionId, sid[1]: "
             << platform_session_id.session_id[1];
  std::string web_session_id = "";

  std::map<std::string, OpenCdmPlatformSessionId>::const_iterator it;
  for (it = session_id_map.begin(); it != session_id_map.end(); it++) {
    if (it->second.session_id_len == platform_session_id.session_id_len) {
      bool equal = false;
      for (uint32_t i_cmp = 0; i_cmp < it->second.session_id_len; i_cmp++) {
        equal = (it->second.session_id[i_cmp]
            == platform_session_id.session_id[i_cmp]);
        if (!equal) {
          break;
        }
      }
      if (equal) {
        web_session_id = it->first;
        break;
      }
    }
  }

  return web_session_id;
}

cdm::Status OpenCdm::DecryptToMediaDecoderBuffer(
    const cdm::InputBuffer& encrypted_buffer,
    scoped_refptr<media::DecoderBuffer>* decrypted_buffer) {
  CDM_DLOG() << "OpenCdm::DecryptToMediaDecoderBuffer()";

  DCHECK(decrypted_buffer);
  scoped_refptr<media::DecoderBuffer> buffer = CopyDecoderBufferFrom(
      encrypted_buffer);

  if (buffer->end_of_stream()) {
    CDM_DLOG() << "#buffer->end_of_stream()";
    *decrypted_buffer = buffer;
    return cdm::kSuccess;
  }

  // from AESDecryptor.decrypt
  // An empty iv string signals that the frame is unencrypted.
  if (buffer->decrypt_config()->iv().empty()) {
    CDM_DLOG() << "#buffer->decrypt_config()->iv().empty()";
    *decrypted_buffer = buffer;
    return cdm::kSuccess;
  }

  // mediaengine instantiation
  if (!media_engine_) {
    // TODO(ska): handle mutiple sessions
    OpenCdmPlatformSessionId session_id = GetPlatformSessionId(
        last_session_id_);
    media_engine_ = OpenCdmMediaengineFactory::Create(key_system_, session_id);
    CDM_DLOG() << "new MediaEngineSession instantiated";
  }

  // from AESDecryptor.decryptData

  const char* sample = reinterpret_cast<const char*>(buffer.get()->data());
  size_t sample_size = static_cast<size_t>(buffer.get()->data_size());

  if (sample_size == 0) {
    CDM_DLOG() << "sample_size == 0";
    return cdm::kDecryptError;
  }

  if (buffer.get()->decrypt_config()->subsamples().empty()) {
    CDM_DLOG() << "#subsamples().empty()";
  }

  const std::vector<SubsampleEntry>& subsamples = buffer.get()->decrypt_config()
      ->subsamples();

  size_t total_clear_size = 0;
  size_t total_encrypted_size = 0;
  for (size_t i = 0; i < subsamples.size(); i++) {
    total_clear_size += subsamples[i].clear_bytes;
    total_encrypted_size += subsamples[i].cypher_bytes;
    // Check for overflow. This check is valid because *_size is unsigned.
    DCHECK(total_clear_size >= subsamples[i].clear_bytes);
    if (total_encrypted_size < subsamples[i].cypher_bytes) {
      CDM_DLOG() << "#total_encrypted_size < subsamples[i].cypher_bytes";
      return cdm::kDecryptError;
    }
  }
  size_t total_size = total_clear_size + total_encrypted_size;
  if (total_size < total_clear_size || total_size != sample_size) {
    CDM_DLOG() << "#Subsample sizes do not equal input size";
    return cdm::kDecryptError;
  }

  // No need to decrypt if there is no encrypted data.
  if (total_encrypted_size <= 0) {
    CDM_DLOG() << "#No need to decrypt no encrypted data";
    *decrypted_buffer = DecoderBuffer::CopyFrom(
        reinterpret_cast<const uint8*>(sample), sample_size);
    return cdm::kSuccess;
  }

  // The encrypted portions of all subsamples must form a contiguous block,
  // such that an encrypted subsample that ends away from a block boundary is
  // immediately followed by the start of the next encrypted subsample. We
  // copy all encrypted subsamples to a contiguous buffer, decrypt them, then
  // copy the decrypted bytes over the encrypted bytes in the output.
  // TODO(strobe): attempt to reduce number of memory copies
  scoped_ptr<uint8[]> encrypted_bytes(new uint8[total_encrypted_size]);
  CopySubsamples(subsamples, kSrcContainsClearBytes,
                 reinterpret_cast<const uint8*>(sample), encrypted_bytes.get());

  uint8_t * out = new uint8_t[encrypted_buffer.data_size];
  uint32_t out_size = -1;

  DecryptResponse dr = media_engine_->Decrypt(encrypted_buffer.iv,
                                              encrypted_buffer.iv_size,
                                              encrypted_bytes.get(),
                                              total_encrypted_size, out,
                                              out_size);
  CDM_DLOG() << "media_engine_->Decrypt done";

  DCHECK_EQ(out_size, total_encrypted_size);

  scoped_refptr<DecoderBuffer> output = DecoderBuffer::CopyFrom(
      reinterpret_cast<const uint8*>(sample), sample_size);
  CopySubsamples(subsamples, kDstContainsClearBytes, out,
                 output->writable_data());

  delete[] out;

  *decrypted_buffer = output;
  if (dr.platform_response == PLATFORM_CALL_SUCCESS) {
    return cdm::kSuccess;
  } else {
    return cdm::kDecryptError;
  }
}

void OpenCdm::OnPlatformChallengeResponse(
    const cdm::PlatformChallengeResponse& response) {
  CDM_DLOG() << "OpenCdm::OnPlatformChallengeResponse";
  NOTIMPLEMENTED();
}

void OpenCdm::OnQueryOutputProtectionStatus(uint32_t link_mask,
                                            uint32_t output_protection_mask) {
  CDM_DLOG() << "OpenCdm::OnQueryOutputProtectionStatus";
  NOTIMPLEMENTED();
}

void OpenCdm::OnSessionMessage(const std::string& web_session_id,
                               const std::vector<uint8>& message,
                               const GURL& destination_url) {
  CDM_DLOG() << "OpenCdm::OnSessionMessage: " << web_session_id.data();

  // OnSessionMessage() only called during CreateSession(), so no promise
  // involved (OnSessionCreated() called to resolve the CreateSession()
  // promise).
  host_->OnSessionMessage(web_session_id.data(), web_session_id.length(),
                          reinterpret_cast<const char*>(message.data()),
                          message.size(), destination_url.spec().data(),
                          destination_url.spec().size());
}

void OpenCdm::OnSessionReady(const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::OnSessionReady " << web_session_id.data();
  host_->OnSessionReady(web_session_id.data(), web_session_id.length());
  if (last_stream_type_ == cdm::kStreamTypeAudio) {
    // TODO(ska): get stream type for session id
    if (audio_decoder_state_ == cdm::kDeferredInitialization) {
      CDM_DLOG()
          << "call host.OnDeferredInitializationDone for "
          << ((last_stream_type_ == cdm::kStreamTypeAudio) ? "audio" : "video");
      host_->OnDeferredInitializationDone(last_stream_type_, cdm::kSuccess);
      audio_decoder_state_ = cdm::kSuccess;
    }
  } else if (last_stream_type_ == cdm::kStreamTypeVideo) {
    if (video_decoder_state_ == cdm::kDeferredInitialization) {
      CDM_DLOG()
          << "call host.OnDeferredInitializationDone for "
          << ((last_stream_type_ == cdm::kStreamTypeAudio) ? "audio" : "video");
      host_->OnDeferredInitializationDone(last_stream_type_, cdm::kSuccess);
      video_decoder_state_ = cdm::kSuccess;
    }
  }
}

void OpenCdm::OnSessionError(const std::string& web_session_id,
                             MediaKeys::Exception exception_code,
                             uint32 system_code,
                             const std::string& error_message) {
  CDM_DLOG() << "OnSessionError " << web_session_id.data();
  // exception_code is always unknown -> kUnknownError is reported
  host_->OnSessionError(web_session_id.data(), web_session_id.length(),
                        cdm::kUnknownError, system_code, error_message.data(),
                        error_message.length());
}

void OpenCdm::OnSessionCreated(uint32 promise_id,
                               const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::OnSessionCreated";
  // Save the latest session ID for heartbeat and file IO test messages.
  last_session_id_ = web_session_id;

  host_->OnResolveNewSessionPromise(promise_id, web_session_id.data(),
                                    web_session_id.length());
}

void OpenCdm::OnSessionLoaded(uint32 promise_id,
                              const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::OnSessionLoaded";
}

void OpenCdm::OnSessionUpdated(uint32 promise_id,
                               const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::OnSessionUpdated";
  host_->OnResolvePromise(promise_id);
}

void OpenCdm::OnSessionReleased(uint32 promise_id,
                                const std::string& web_session_id) {
  CDM_DLOG() << "OpenCdm::OnSessionReleased";
  host_->OnResolvePromise(promise_id);
}

void OpenCdm::OnPromiseFailed(uint32 promise_id,
                              MediaKeys::Exception exception_code,
                              uint32 system_code,
                              const std::string& error_message) {
  CDM_DLOG() << "OpenCdm::OnPromiseFailed";

  host_->OnRejectPromise(promise_id, ConvertException(exception_code),
                         system_code, error_message.data(),
                         error_message.length());
}

}  // namespace media