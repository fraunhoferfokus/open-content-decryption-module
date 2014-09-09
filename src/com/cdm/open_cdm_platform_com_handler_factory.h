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

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_OPEN_CDM_PLATFORM_COM_HANDLER_FACTORY_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_OPEN_CDM_PLATFORM_COM_HANDLER_FACTORY_H_

#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com.h"
#include "media/cdm/ppapi/external_open_cdm/cdm/open_cdm_platform_com_callback_receiver.h"
#include "media/cdm/ppapi/external_open_cdm/com/cdm/rpc/rpc_cdm_platform_handler.h"

namespace media {

class OpenCdmPlatformComHandlerFactory {
 public:
  static OpenCdmPlatformCom *Create(
      OpenCdmPlatformComCallbackReceiver *callback_receiver_);
};

/*
 * returns raw pointer to be platform and type independent, can be converted
 * to smart pointer on successful creation.
 */
OpenCdmPlatformCom *OpenCdmPlatformComHandlerFactory::Create(
    OpenCdmPlatformComCallbackReceiver *callback_receiver_) {
  return new RpcCdmPlatformHandler(callback_receiver_);
}
}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_COM_CDM_OPEN_CDM_PLATFORM_COM_HANDLER_FACTORY_H_
