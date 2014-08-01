# Build notes for Chromium

Chromium supports three types of CDMs:

* Internal from source (Clear Key, which is implemented internally by aes_decryptor.cc)
* Pepper-based CDMs (used for desktop and Chrome OS)
* Platform-based (e.g. Android using MediaDrm APIs)

Source: [David Dorwin, Feb 2014](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/exotX6Nf_z0/UtMi4a2sLncJ)

## Internal source integration notes

[tbd]

## Pepper Plugin API (PPAPI) integration notes

*Tested with version:*
MAJOR=38
MINOR=0
BUILD= **2041**
PATCH=0

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
to be registered by adding and calling the following function:

```
// External Open CDM.
static void AddExternalOpenCdm(
    std::vector<KeySystemInfo>* concrete_key_systems) {
  static const char kExternalOpenCdmKeySystem[] =
      "com.youdomainnamehere.yourcdmname";
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

Finally, add the following call into the ```AddChromeKeySystems``` function
subsequent the check if Pepper CDMs are enabled:

```
if defined(ENABLE_PEPPER_CDMS)
  AddExternalClearKey(key_systems_info);
  AddExternalOpenCdm(key_systems_info);
```

## Platform based integration notes

[tbd]
