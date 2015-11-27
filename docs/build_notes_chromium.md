# Build notes for Chromium

Chromium supports three types of CDMs:

* Internal from source (Clear Key, which is implemented internally by aes_decryptor.cc)
* Pepper-based CDMs (used for desktop and Chrome OS)
* Platform-based (e.g. Android using MediaDrm APIs)

Source: [David Dorwin, Feb 2014](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/exotX6Nf_z0/UtMi4a2sLncJ)

## Pepper Plugin API (PPAPI) integration notes

*Tested with versions:*
<br />
MAJOR=37
MINOR=0
BUILD= **2041**
PATCH=6
<br />
MAJOR=43
MINOR=0
BUILD= **2357**
PATCH=81

## Configuration

Make sure to have the following gyp variables set before building.
Easiest way is to place a ```~/.gyp/include.gypi``` with contents:
```
{
  'variables': {
    'ffmpeg_branding': 'Chrome',
    'proprietary_codecs': 1,
    'enable_pepper_cdms': 1
  }
}
```

## Code hooks

To make the CDM available in the chromium build the key system needs to be
registered in the file ```$CHROMIUM_ROOT/src/chrome/renderer/media/chrome_key_systems.cc```

Similar as for the function ```AddExternalClearKey``` the new key system needs
to be registered by adding and calling the following function that depends on the version of chromium you are using.

### Chromium Version 37.0.2041.6

```
// External Open CDM.
static void AddExternalOpenCdm(
    std::vector<KeySystemInfo>* concrete_key_systems) {
  static const char kExternalOpenCdmKeySystem[] =
      "com.opencdm.mockdrm";
  static const char kExternalOpenCdmPepperType[] =
      "application/x-ppapi-open-cdm";

  std::vector<base::string16> additional_param_names;
  std::vector<base::string16> additional_param_values;
  if (!IsPepperCdmAvailable(kExternalOpenCdmPepperType,
                             &additional_param_names,
                             &additional_param_values)) {
    return;
  }

  KeySystemInfo info(kExternalOpenCdmKeySystem);

#if defined(USE_PROPRIETARY_CODECS)
  info.supported_codecs |= content::EME_CODEC_MP4_ALL;
#endif  // defined(USE_PROPRIETARY_CODECS)

  info.pepper_type = kExternalOpenCdmPepperType;

  concrete_key_systems->push_back(info);
}
```

### Chromium Version 43.0.2357.81

```
// External Open CDM.
static void AddExternalOpenCdm(
    std::vector<KeySystemInfo>* concrete_key_systems) {
  static const char kExternalOpenCdmKeySystem[] =
      "com.opencdm.mockdrm";
  static const char kExternalOpenCdmPepperType[] =
      "application/x-ppapi-open-cdm";

  std::vector<base::string16> additional_param_names;
  std::vector<base::string16> additional_param_values;
  if (!IsPepperCdmAvailable(kExternalOpenCdmPepperType,
                             &additional_param_names,
                             &additional_param_values)) {
    return;
  }

  KeySystemInfo info;
  info.key_system = kExternalOpenCdmKeySystem;
  info.supported_codecs |= media::EME_CODEC_WEBM_ALL;
  info.supported_init_data_types |= media::kInitDataTypeMaskWebM;
  #if defined (USE_PROPRIETARY_CODECS)
  info.supported_codecs |= media::EME_CODEC_MP4_ALL;
  info.supported_init_data_types |= media::kInitDataTypeMaskCenc;
  #endif


  info.pepper_type = kExternalOpenCdmPepperType;

  concrete_key_systems->push_back(info);
}
```

### Both Chromium Version 37.0.2041.6 and 43.0.2357.81

Finally, add the following call into the ```AddChromeKeySystems``` function
subsequent the check if Pepper CDMs are enabled:

```
if defined(ENABLE_PEPPER_CDMS)
  AddExternalClearKey(key_systems_info);
  AddExternalOpenCdm(key_systems_info);
```
