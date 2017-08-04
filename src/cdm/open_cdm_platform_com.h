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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_H_

#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_common.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com_callback_receiver.h"

#include <string>

namespace media {

enum OCDM_COM_STATE {
  UNINITIALIZED = 0,
  INITIALIZED = 1,
  FAULTY = 2,
  ABORTED = 3
};

class OpenCdmPlatformCom : public OpenCdmPlatform {
 public:
  ~OpenCdmPlatformCom() override{
  }
  OpenCdmPlatformCom(OpenCdmPlatformComCallbackReceiver *callback_receiver_);

 protected:
  OpenCdmPlatformCom() {
  }

  OCDM_COM_STATE com_state;

 private:
};
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_CDM_OPEN_CDM_PLATFORM_COM_H_
