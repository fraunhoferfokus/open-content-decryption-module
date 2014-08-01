# Copyright (c) 2014 Fraunhofer FOKUS. All rights reserved.
# Use of this source code is governed by a [tda] license that can be
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
    # Set |use_fake_video_decoder| to 1 to ignore input frames in |clearkeycdm|,
    # and produce video frames filled with a solid color instead.
    'use_fake_video_decoder%': 0,
    # Set |use_libvpx| to 1 to use libvpx for VP8 decoding in |clearkeycdm|.
    'use_libvpx%': 0,
  },
  'conditions': [
    ['enable_pepper_cdms==1', {
      'targets': [
        {
          'target_name': 'open_cdm',
          'type': 'none',
          'conditions': [
            ['use_ffmpeg == 1'  , {
              'defines': ['OPEN_CDM_USE_FFMPEG_DECODER'],
              'dependencies': [
                '<(DEPTH)/third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
              ],
              'sources': [
                #'cdm/ppapi/external_clear_key/ffmpeg_cdm_audio_decoder.cc',
                #'cdm/ppapi/external_clear_key/ffmpeg_cdm_audio_decoder.h',
                #'../decoupled_decoder/open_cdm_ffmpeg_audio_decoder.cc',
                #'../decoupled_decoder/open_cdm_ffmpeg_audio_decoder.h',
              ],
            }],
            ['use_ffmpeg == 1'  , {
              'sources': [
                #'cdm/ppapi/external_clear_key/ffmpeg_cdm_video_decoder.cc',
                #'cdm/ppapi/external_clear_key/ffmpeg_cdm_video_decoder.h',
                #'../decoupled_decoder/open_cdm_ffmpeg_video_decoder.cc',
                #'../decoupled_decoder/open_cdm_ffmpeg_video_decoder.h',
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
            '../com/common/rpc/cdmxdr.h',
            '../com/common/rpc/cdmxdr_clnt.c',
            '../com/common/rpc/cdmxdr_xdr.c',
            '../cdm/cdm_platform_common.h',
            '../cdm/cdm_platform_interface.h',
            '../cdm/cdm_platform_interface_factory.h',
            '../com/cdm/cdm_platform_interface_impl.cc',
            '../com/cdm/cdm_platform_interface_impl.h',
            '../browser/chrome/open_cdm.cc',
            '../browser/chrome/open_cdm.h',
            '../browser/chrome/open_cdm_common.h',
            '../browser/chrome/open_media_keys.cc',
            '../browser/chrome/open_media_keys.h',
            #'../cdmi/shmemsem/shmemsem_helper.cc',
            #'../cdmi/shmemsem/shmemsem_helper.h',
            #'../mediaengine/cdm_mediaengine_interface.h',
            #'../com/mediaengine/cdm_mediaengine_interface_impl.cc',
            #'../com/mediaengine/cdm_mediaengine_interface_impl.h',
          ],
          # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
          'msvs_disabled_warnings': [ 4267, ],
        },
        {
          'target_name': 'open_cdmadapter',
          'type': 'none',
          # Check whether the plugin's origin URL is valid.
          'defines': ['CHECK_DOCUMENT_URL'],
          'dependencies': [
            '<(DEPTH)/ppapi/ppapi.gyp:ppapi_cpp',
            '<(DEPTH)/media/media_cdm_adapter.gyp:cdmadapter',
            'open_cdm',
          ],
          'conditions': [
            ['os_posix == 1 and OS != "mac" and enable_pepper_cdms==1', {
              # Because open_cdm has type 'loadable_module' (see comments),
              # we must explicitly specify this dependency.
              'libraries': [
                # Built by open_cdm.
                '<(PRODUCT_DIR)/libopen_cdm.so',
              ],
            }],
          ],
        },
      ],
    }],
  ],
}
