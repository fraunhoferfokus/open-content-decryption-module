## How to test

### ...with Pepper Plugin for Chromium

* Test setup:
 * ```$ cd $CHROMIUM_ROOT/src```
 * ```$ ln -s $HOME/opencdm/src/browser/chrome/tests/data $CHROMIUM_ROOT/src/chrome/test/data/media/drmock```
 * In file $CHROMIUM_ROOT/src/chrome/chrome_tests.gypi add the following to the sources section of the target browser_tests
 
```
  'browser/media/encrypted_media_istypesupported_browsertest.cc',
  # OCDM tests
  '<(DEPTH)/media/cdm/ppapi/external_open_cdm/browser/chrome/tests/ocdm_encrypted_media_istypesupported_browsertest.cc',
```

 * ```$ ./build/gyp_chromium```
 * ```$ ninja -C out/Debug browser_tests```

* Test execution:
 * ```$ cd $CHROMIUM_ROOT/src```
 * ```$ ./out/Debug/browser_tests --gtest_filter="OpenCDM*"```
