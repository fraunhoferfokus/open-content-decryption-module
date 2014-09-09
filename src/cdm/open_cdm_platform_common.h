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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COMMON_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COMMON_H_

#include <string>
#include "base/basictypes.h"

namespace media {

// TODO(sph): PLATFORM_CALL_STATE should not be CDMi, but EME-specific
enum PLATFORM_CALL_STATE {
  PLATFORM_CALL_SUCCESS = 0,
  PLATFORM_CALL_FAIL = 1,
  PLATFORM_CALL_NOACTION = 2,
  PLATFORM_CALL_NEEDKEY = 3
};

struct PlatformResponse {
  PLATFORM_CALL_STATE platform_response;
  int32 sys_err;
};

struct OpenCdmPlatformSessionId {
  uint16_t *session_id;  // TODO(ska): uint16_t oder uint8_t ? CDMi: 16
  uint32_t session_id_len;
};

struct MediaKeysResponse : public PlatformResponse {
};
struct MediaKeysCreateSessionResponse : public PlatformResponse {
  OpenCdmPlatformSessionId session_id;
};
struct MediaKeysLoadSessionResponse : public PlatformResponse {
};
struct MediaKeySessionUpdateResponse : public PlatformResponse {
};
struct MediaKeySessionReleaseResponse : public PlatformResponse {
};

class DecryptReadyStateKeeper {
 public:
  virtual ~DecryptReadyStateKeeper();
  static DecryptReadyStateKeeper *getInstance();

  void lockDecryptSem();
  void unlockDecryptSem();
  void lockInitSem();
  void unlockInitSem();
 private:
  DecryptReadyStateKeeper();
  static DecryptReadyStateKeeper *instance;

  int idDecryptReadySem;
  int _instance_id;
};

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COMMON_H_
