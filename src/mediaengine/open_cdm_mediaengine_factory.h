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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_FACTORY_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_FACTORY_H_

#include <string>

#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine.h"
#include "media/cdm/ppapi/external_open_cdm/mediaengine/open_cdm_mediaengine_impl.h"

#include "media/cdm/ppapi/cdm_logging.h"

namespace media {

//const std::string open_cdm_key_system = "com.opencdm.mockdrm";
const std::string open_cdm_key_system = "org.chromium.externalclearkey";

// TODO(ska): outsource the mapping of key system string
// to mediaengine and platform implementations

class OpenCdmMediaengineFactory {
 public:
  static OpenCdmMediaengine *Create(std::string key_system,
                                    OpenCdmPlatformSessionId session_id);
};

/*
 * returns raw pointer to be platform and type independent, can be converted
 * to smart pointer on successful creation.
 */
OpenCdmMediaengine *OpenCdmMediaengineFactory::Create(
    std::string key_system, OpenCdmPlatformSessionId session_id) {
  if (key_system == open_cdm_key_system) {
    CDM_DLOG() << "Instantiate OpenCdmMediaengineImpl!";
    return new OpenCdmMediaengineImpl(session_id.session_id,
                                      session_id.session_id_len);
  } else {
    CDM_DLOG() << "Failed to create MediaEngine";
    return NULL;
  }
}
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_MEDIAENGINE_OPEN_CDM_MEDIAENGINE_FACTORY_H_
