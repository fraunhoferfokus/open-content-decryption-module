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
// media/cdm/ppapi/external_clear_key/clear_key_cdm_common.h
// License notice of original file:

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_CHROME_COMMON_H_
#define MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_CHROME_COMMON_H_

#include "media/cdm/ppapi/api/content_decryption_module.h"

namespace media {

// Aliases for the version of the interfaces that this CDM implements.
typedef cdm::ContentDecryptionModule_5 OpenCdmInterface;
typedef OpenCdmInterface::Host OpenCdmHost;

}  // namespace media

#endif  // MEDIA_CDM_PPAPI_EXTERNAL_OPEN_CDM_BROWSER_CHROME_OPEN_CDM_CHROME_COMMON_H_
