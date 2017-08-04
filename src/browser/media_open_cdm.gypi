#
# Copyright (c) 2017 Fraunhofer FOKUS
#
# Licensed under the MIT License (the "License");
# you may not use this file except in compliance with the License.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Based on file contibuted to the Chromium project
# media/media_cdm.gypi
# License notice of original file:

# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'conditions': [
      ['OS == "android"', {
        # Android doesn't use ffmpeg.
        'use_ffmpeg%': 0,
      }, {  # 'OS != "android"'
        'use_ffmpeg%': 1,
      }],
    ],
  },
  'conditions': [
    ['enable_pepper_cdms==1', {
      'targets': [
        {
          'target_name': 'opencdm',
          'type': 'none',
          'conditions': [
            ['use_ffmpeg == 1'  , {
              'defines': ['OCDM_USE_FFMPEG_DECODER', 'CLEAR_KEY_CDM_USE_FFMPEG_DECODER'],
              'dependencies': [
                '<(DEPTH)/third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
              ],
              'sources': [
                '<(DEPTH)/media/cdm/ppapi/external_clear_key/ffmpeg_cdm_audio_decoder.cc',
                '<(DEPTH)/media/cdm/ppapi/external_clear_key/ffmpeg_cdm_audio_decoder.h',
              ],
            }],
            ['use_ffmpeg == 1'  , {
              'sources': [
                '<(DEPTH)/media/cdm/ppapi/external_clear_key/ffmpeg_cdm_video_decoder.cc',
                '<(DEPTH)/media/cdm/ppapi/external_clear_key/ffmpeg_cdm_video_decoder.h',
              ],
            }],
            ['os_posix == 1 and OS != "mac" and enable_pepper_cdms==1', {
              'type': 'loadable_module',  # Must be in PRODUCT_DIR for ASAN bot.
            }],
            ['(OS == "mac" or OS == "win") and enable_pepper_cdms==1', {
              'type': 'shared_library',
            }],
            ['OS == "mac"', {
              'xcode_settings': {
                'DYLIB_INSTALL_NAME_BASE': '@loader_path',
              },
            }]
          ],
          'defines': ['CDM_IMPLEMENTATION'],
          'dependencies': [
            'media',
            '../url/url.gyp:url_lib',
            # Include the following for media::AudioBus.
            'shared_memory_support',
            '<(DEPTH)/base/base.gyp:base',
          ],
          'sources': [
            '<(DEPTH)/media/cdm/ppapi/cdm_logging.cc',
            '<(DEPTH)/media/cdm/ppapi/cdm_logging.h',
            '../com/common/rpc/opencdm_xdr.h',
            '../com/common/rpc/opencdm_xdr_clnt.c',
            '../com/common/rpc/opencdm_xdr_xdr.c',
            '../com/common/rpc/opencdm_callback.h',
            '../com/common/rpc/opencdm_callback_xdr.c',
            '../cdm/open_cdm_platform_common.h',
            '../cdm/open_cdm_platform.h',
            '../cdm/open_cdm_platform_com.h',
            '../cdm/open_cdm_platform_factory.h',
            '../cdm/open_cdm_platform_com_callback_receiver.h',
            '../cdm/open_cdm_platform_impl.cc',
            '../cdm/open_cdm_platform_impl.h',
            '../com/cdm/open_cdm_platform_com_handler_factory.h',
            '../com/cdm/rpc/rpc_cdm_platform_handler.h',
            '../com/cdm/rpc/rpc_cdm_platform_handler.cc',
            '../com/common/shmemsem/shmemsem_helper.cc',
            '../com/common/shmemsem/shmemsem_helper.h',
            '../mediaengine/open_cdm_mediaengine.h',
            '../mediaengine/open_cdm_mediaengine_com.h',
            '../mediaengine/open_cdm_mediaengine_factory.h',
            '../mediaengine/open_cdm_mediaengine_impl.cc',
            '../mediaengine/open_cdm_mediaengine_impl.h',
            '../com/mediaengine/rpc/rpc_cdm_mediaengine_handler.cc',
            '../com/mediaengine/rpc/rpc_cdm_mediaengine_handler.h',
            '../browser/chrome/open_cdm.cc',
            '../browser/chrome/open_cdm.h',
            '../browser/chrome/open_cdm_chrome_common.h',
            '../common/open_cdm_common.h',
            '<(DEPTH)/media/cdm/ppapi/external_clear_key/cdm_video_decoder.cc',
            '<(DEPTH)/media/cdm/ppapi/external_clear_key/cdm_video_decoder.h',
          ],
          # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
          'msvs_disabled_warnings': [ 4267, ],
        },
        {
          'target_name': 'opencdmadapter',
          'type': 'none',
          # Check whether the plugin's origin URL is valid.
          'defines': ['CHECK_DOCUMENT_URL'],
          'dependencies': [
            '<(DEPTH)/ppapi/ppapi.gyp:ppapi_cpp',
            '<(DEPTH)/media/media_cdm_adapter.gyp:cdmadapter',
            'opencdm',
          ],
          'conditions': [
            ['os_posix == 1 and OS != "mac" and enable_pepper_cdms==1', {
              # Because opencdm has type 'loadable_module' (see comments),
              # we must explicitly specify this dependency.
              'libraries': [
                # Built by opencdm.
                '<(PRODUCT_DIR)/libopencdm.so',
              ],
            }],
          ],
        },
      ],
    }],
  ],
}