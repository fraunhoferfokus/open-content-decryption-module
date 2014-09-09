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
// chrome/browser/media/encrypted_media_istypesupported_browsertest.cc
// License notice of original file:

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/browser_test_utils.h"
#include "url/gurl.h"

#if defined(OS_ANDROID)
#error This file needs to be updated to run on Android.
#endif

#if defined(USE_PROPRIETARY_CODECS)
#define EXPECT_PROPRIETARY EXPECT_TRUE
#else
#define EXPECT_PROPRIETARY EXPECT_FALSE
#endif

// Expectations for OpenCDM.
#define EXPECT_OCDM EXPECT_TRUE
#define EXPECT_OCDMPROPRIETARY EXPECT_PROPRIETARY

namespace chrome {

#if defined(ENABLE_PEPPER_CDMS)

const char kOpenCDM[] = "com.opencdm.mockdrm";
const char kOpenCDMParentOnly[] = "com.opencdm";
const char kOpenCDMParentOnlyPeriod[] = "com.opencdm.";
const char kOpenCDMTldPeriod[] = "com.";
const char kOpenCDMTld[] = "com";
const char kOpenCDMCaseSensitive[] = "com.opencdm.MoCkDrM";
const char kOpenCDMNonExisting[] = "com.opencdm.foo";
const char kOpenCDMChild[] = "com.opencdm.mockdrm.foo";
const char kOpenCDMExtraChar[] = "com.opencdm.mockdrmZ";
const char kOpenCDMLessChar[] = "com.opencdm.mockdr";
const char kPepperTypeForKeySystem[] = "application/x-ppapi-open-cdm";
const char kFilePathLiteral[] = "#OpenCDM#1.0.0.0;";

// TODO(xhwang): Simplify this test! See http://crbug.com/367158

class OpenCDMEncryptedMediaIsTypeSupportedBaseTest : 
 public InProcessBrowserTest {
 protected:
  OpenCDMEncryptedMediaIsTypeSupportedBaseTest()
      : is_test_page_loaded_(false), is_pepper_cdm_registered_(false) {
    vp8_codec_.push_back("vp8");

    vp80_codec_.push_back("vp8.0");

    vp9_codec_.push_back("vp9");

    vp90_codec_.push_back("vp9.0");

    vorbis_codec_.push_back("vorbis");

    vp8_and_vorbis_codecs_.push_back("vp8");
    vp8_and_vorbis_codecs_.push_back("vorbis");

    vp9_and_vorbis_codecs_.push_back("vp9");
    vp9_and_vorbis_codecs_.push_back("vorbis");

    avc1_codec_.push_back("avc1");

    avc1_extended_codec_.push_back("avc1.4D400C");

    avc1_dot_codec_.push_back("avc1.");

    avc2_codec_.push_back("avc2");

    avc3_codec_.push_back("avc3");

    avc3_extended_codec_.push_back("avc3.64001f");

    aac_codec_.push_back("mp4a");

    avc1_and_aac_codecs_.push_back("avc1");
    avc1_and_aac_codecs_.push_back("mp4a");

    unknown_codec_.push_back("foo");

    mixed_codecs_.push_back("vorbis");
    mixed_codecs_.push_back("avc1");
  }

  typedef std::vector<std::string> CodecVector;

  const CodecVector& no_codecs() const { return no_codecs_; }
  const CodecVector& vp8_codec() const { return vp8_codec_; }
  const CodecVector& vp80_codec() const { return vp80_codec_; }
  const CodecVector& vp9_codec() const { return vp9_codec_; }
  const CodecVector& vp90_codec() const { return vp90_codec_; }
  const CodecVector& vorbis_codec() const { return vorbis_codec_; }
  const CodecVector& vp8_and_vorbis_codecs() const {
    return vp8_and_vorbis_codecs_;
  }
  const CodecVector& vp9_and_vorbis_codecs() const {
    return vp9_and_vorbis_codecs_;
  }
  const CodecVector& avc1_codec() const { return avc1_codec_; }
  const CodecVector& avc1_extended_codec() const {
    return avc1_extended_codec_;
  }
  const CodecVector& avc1_dot_codec() const { return avc1_dot_codec_; }
  const CodecVector& avc2_codec() const { return avc2_codec_; }
  const CodecVector& avc3_codec() const { return avc3_codec_; }
  const CodecVector& avc3_extended_codec() const {
    return avc3_extended_codec_;
  }
  const CodecVector& aac_codec() const { return aac_codec_; }
  const CodecVector& avc1_and_aac_codecs() const {
    return avc1_and_aac_codecs_;
  }
  const CodecVector& unknown_codec() const { return unknown_codec_; }
  const CodecVector& mixed_codecs() const { return mixed_codecs_; }

  // Update the command line to load |adapter_name| for
  // |pepper_type_for_key_system|.
  void RegisterPepperCdm(CommandLine* command_line,
                         const std::string& adapter_name,
                         const std::string& pepper_type_for_key_system,
                         bool expect_adapter_exists = true) {
    DCHECK(!is_pepper_cdm_registered_)
        << "RegisterPepperCdm() can only be called once.";
    is_pepper_cdm_registered_ = true;

    // Append the switch to register the appropriate adapter.
    base::FilePath plugin_dir;
    EXPECT_TRUE(PathService::Get(base::DIR_MODULE, &plugin_dir));
    base::FilePath plugin_lib = plugin_dir.AppendASCII(adapter_name);
    EXPECT_EQ(expect_adapter_exists, base::PathExists(plugin_lib));
    base::FilePath::StringType pepper_plugin = plugin_lib.value();
    pepper_plugin.append(FILE_PATH_LITERAL(kFilePathLiteral));
#if defined(OS_WIN)
    pepper_plugin.append(base::ASCIIToWide(pepper_type_for_key_system));
#else
    pepper_plugin.append(pepper_type_for_key_system);
#endif
    command_line->AppendSwitchNative(switches::kRegisterPepperPlugins,
                                     pepper_plugin);
    // TODO(mla): remove after communication to cdmi is possible 
    //            from within the sandbox
    command_line->AppendSwitch(switches::kNoSandbox);
    // TODO(mla): swich needed until unprefixed EME version available 
    command_line->AppendSwitch(switches::kEnableEncryptedMedia);
  }

  void LoadTestPage() {
    // Load the test page needed. IsConcreteSupportedKeySystem() needs some
    // JavaScript and a video loaded in order to work.
    if (!is_test_page_loaded_) {
      ASSERT_TRUE(test_server()->Start());
      GURL gurl = test_server()->GetURL(
          "files/media/drmock/test.html");
      ui_test_utils::NavigateToURL(browser(), gurl);
      is_test_page_loaded_ = true;
    }
  }

  bool IsConcreteSupportedKeySystem(const std::string& key) {
    std::string command(
        "window.domAutomationController.send(MediaKeys_isTypeSupported('");
    command.append(key);
    command.append("'));");

    // testKeySystemInstantiation() is a JavaScript function which needs to
    // be loaded.
    LoadTestPage();

    std::string result;
    EXPECT_TRUE(content::ExecuteScriptAndExtractString(
        browser()->tab_strip_model()->GetActiveWebContents(),
        command,
        &result));
    CHECK(result == "success" || result == "NotSupportedError") << result;
    return (result == "success");
  }

  bool IsSupportedKeySystemWithMediaMimeType(const std::string& type,
                                             const CodecVector& codecs,
                                             const std::string& keySystem) {
    std::string command("document.createElement('video').canPlayType(");
    if (type.empty()) {
      // Simple case, pass "null" as first argument.
      command.append("null");
      DCHECK(codecs.empty());
    } else {
      command.append("'");
      command.append(type);
      if (!codecs.empty()) {
        command.append("; codecs=\"");
        for (CodecVector::const_iterator it = codecs.begin();
             it != codecs.end();
             ++it) {
          command.append(*it);
          command.append(",");
        }
        command.replace(command.length() - 1, 1, "\"");
      }
      command.append("'");
    }
    command.append(",'");
    command.append(keySystem);
    command.append("')");

    std::string result;
    EXPECT_TRUE(content::ExecuteScriptAndExtractString(
        browser()->tab_strip_model()->GetActiveWebContents(),
        "window.domAutomationController.send(" + command + ");",
        &result));
    return (result == "maybe" || result == "probably");
  }

 private:
  const CodecVector no_codecs_;
  CodecVector vp8_codec_;
  CodecVector vp80_codec_;
  CodecVector vp9_codec_;
  CodecVector vp90_codec_;
  CodecVector vorbis_codec_;
  CodecVector vp8_and_vorbis_codecs_;
  CodecVector vp9_and_vorbis_codecs_;
  CodecVector avc1_codec_;
  CodecVector avc1_extended_codec_;
  CodecVector avc1_dot_codec_;
  CodecVector avc2_codec_;
  CodecVector avc3_codec_;
  CodecVector avc3_extended_codec_;
  CodecVector aac_codec_;
  CodecVector avc1_and_aac_codecs_;
  CodecVector unknown_codec_;
  CodecVector mixed_codecs_;
  bool is_test_page_loaded_;
  bool is_pepper_cdm_registered_;
};

// For PPAPI OpenCDM tests, ensure that the OpenCDM adapter is loaded.
class OpenCDMEncryptedMediaIsTypeSupportedTest
    : public OpenCDMEncryptedMediaIsTypeSupportedBaseTest {

 protected:
  virtual void SetUpCommandLine(CommandLine* command_line) OVERRIDE {
    // Platform-specific filename relative to the chrome executable.
    const char adapter_file_name[] =
#if defined(OS_MACOSX)
        "opencdmadapter.plugin";
#elif defined(OS_WIN)
        "opencdmadapter.dll";
#elif defined(OS_POSIX)
        "libopencdmadapter.so";
#endif

    const std::string pepper_name(kPepperTypeForKeySystem);
    RegisterPepperCdm(command_line, adapter_file_name, pepper_name);
  }

};

// Registers OpenCDM with the wrong path (filename).
class OpenCDMEncryptedMediaIsTypeSupportedRegisteredWithWrongPathTest
    : public OpenCDMEncryptedMediaIsTypeSupportedBaseTest {
 protected:
  virtual void SetUpCommandLine(CommandLine* command_line) OVERRIDE {
   RegisterPepperCdm(command_line,
                     "libopencdmadapterwrongname.so",
                     kPepperTypeForKeySystem,
                     false);
  }
};

//
// OpenCDM Tests
//

// When defined(ENABLE_PEPPER_CDMS), this also tests the Pepper CDM check.
IN_PROC_BROWSER_TEST_F(OpenCDMEncryptedMediaIsTypeSupportedTest,
                       OpenCDM_Basic) {
  EXPECT_OCDM(IsConcreteSupportedKeySystem(kOpenCDM));
  EXPECT_OCDM(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDM));
}

IN_PROC_BROWSER_TEST_F(OpenCDMEncryptedMediaIsTypeSupportedTest,
                       OpenCDM_Parent) {
  // The parent should be supported but is not. See http://crbug.com/164303.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMParentOnly));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMParentOnly));
}

IN_PROC_BROWSER_TEST_F(OpenCDMEncryptedMediaIsTypeSupportedTest,
                       OpenCDM_IsSupportedKeySystem_InvalidVariants) {
  // Case sensitive.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMCaseSensitive));
  // This should fail, but currently canPlayType() converts it to lowercase.
  // See http://crbug.com/286036.
  EXPECT_TRUE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMCaseSensitive));

  // TLDs are not allowed.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMTldPeriod));
  EXPECT_FALSE(
      IsSupportedKeySystemWithMediaMimeType("video/mp4", no_codecs(), kOpenCDMTldPeriod));
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMTld));
  EXPECT_FALSE(
      IsSupportedKeySystemWithMediaMimeType("video/mp4", no_codecs(), kOpenCDMTld));

  // Extra period.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMParentOnlyPeriod));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMParentOnlyPeriod));

  // Incomplete.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMLessChar));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMLessChar));

  // Extra character.
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDMExtraChar));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMExtraChar));

  // There are no child key systems for OpenCDM.
  EXPECT_FALSE(
      IsConcreteSupportedKeySystem(kOpenCDMChild));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMChild));
}

IN_PROC_BROWSER_TEST_F(
    OpenCDMEncryptedMediaIsTypeSupportedTest,
    IsSupportedKeySystemWithMediaMimeType_OpenCDM_NoType) {
  // These two should be true. See http://crbug.com/164303.
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      std::string(), no_codecs(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      std::string(), no_codecs(), kOpenCDMParentOnly));

  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      std::string(), no_codecs(), kOpenCDMNonExisting));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      std::string(), no_codecs(), kOpenCDMChild));
}

IN_PROC_BROWSER_TEST_F(
    OpenCDMEncryptedMediaIsTypeSupportedTest,
    IsSupportedKeySystemWithMediaMimeType_OpenCDM_MP4) {
  // Valid video types.
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDM));
  // The parent should be supported but is not. See http://crbug.com/164303.
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDMParentOnly));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc1_codec(), kOpenCDM));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc1_and_aac_codecs(), kOpenCDM));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc3_codec(), kOpenCDM));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", aac_codec(), kOpenCDM));

  // Extended codecs.
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc1_extended_codec(), kOpenCDM));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc3_extended_codec(), kOpenCDM));

  // Invalid codec format, but canPlayType() strips away the period.
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc1_dot_codec(), kOpenCDM));

  // Non-MP4 codecs.
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", avc2_codec(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", vp8_codec(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", unknown_codec(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", mixed_codecs(), kOpenCDM));

  // Valid audio types.
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", no_codecs(), kOpenCDM));
  EXPECT_OCDMPROPRIETARY(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", aac_codec(), kOpenCDM));

  // Non-audio codecs.
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", avc1_codec(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", avc1_and_aac_codecs(), kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", avc3_codec(), kOpenCDM));

  // Non-MP4 codec.
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "audio/mp4", vorbis_codec(), kOpenCDM));
}


// Since this test fixture does not register the CDMs on the command line, the
// check for the CDMs in chrome_key_systems.cc should fail, and they should not
// be registered with KeySystems.
IN_PROC_BROWSER_TEST_F(OpenCDMEncryptedMediaIsTypeSupportedBaseTest,
                       PepperCDMsNotRegistered) {
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDM));
}

// Since this test fixture does not register the CDMs on the command line, the
// check for the CDMs in chrome_key_systems.cc should fail, and they should not
// be registered with KeySystems.
IN_PROC_BROWSER_TEST_F(
    OpenCDMEncryptedMediaIsTypeSupportedRegisteredWithWrongPathTest,
    PepperCDMsRegisteredButAdapterNotPresent) {
  EXPECT_FALSE(IsConcreteSupportedKeySystem(kOpenCDM));
  EXPECT_FALSE(IsSupportedKeySystemWithMediaMimeType(
      "video/mp4", no_codecs(), kOpenCDM));
}

#endif  // defined(ENABLE_PEPPER_CDMS)

}  // namespace chrome
