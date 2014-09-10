# Open Content Decryption Module

The Open Content Decryption Module (OCDM) is a Content Decryption Module (CDM) according to W3C EME [1] specification to be used with HTML5 based browser environments and ecosystems [5]. The implementation enables DRM interoperability.
OCDM is built with a modular concept in mind to be easily extendible and flexible. The open source code already provides support for the CDMi specification [4], which essentially requires the CDM to use the operating system's native RPC mechanisms to forward EME calls to the CDMi.
Furthermore, the media sample transmission between a browser decoupled media engine and the CDMi can be secured via a DRM-specific authenticated interface. The CDMi itself contains most CDM logic and is a c++ wrapper for the embedded platform DRM.

Fraunhofer FOKUS has developed the Open Content Decryption Module (OCDM) according to W3C EME specification to be used with HTML5 based browser environments. The implementation enables DRM interoperability. We would be happy to see a broad adoption of our implementation and encourage contributions. A first e2e implementation has been done testing with a Microsoft PlayReady CDMi implementation.

## Introduction / Purpose / Why this?

* Interoperable HTML5 based protected video delivery
* DRM interoperability
  * CENC, DASH
* Plugin based integration (Pepper Plugin API)
* CDMi allows open source browsers to support DRM without licensing it
* e2e tested with Microsoft PlayReady DRM system

## References

* [1] W3C Encrypted Media Extensions https://dvcs.w3.org/hg/html-media/raw-file/default/encrypted-media/encrypted-media.html
* [2] Interoperability Digital Rights Management and the Web http://www.fokus.fraunhofer.de/en/fame/_pdf/Interoperability_Digital_Rights_Management_and_the_Web.pdf
* [3] MSE-EME Reference Platform Documentation https://html5.cablelabs.com/mse-eme/doc/overview.html
* [4] Content Decryption Module Interface Specification http://download.microsoft.com/download/E/A/4/EA470677-6C3C-4AFE-8A86-A196ADFD0F78/Content%20Decryption%20Module%20Interface%20Specification.pdf
* [5] Fraunhofer FOKUS FAMIUM DRM http://www.fokus.fraunhofer.de/en/fame/Solutions/DRM/index.html

## Supported Browsers and Platforms

Currently OCDM development is targeted for following Web browers:

* Chromium
 * Linux
* Opera
 * Linux
* QtWebengine (coming soon)
 * Linux

For more details see the [milestones](https://github.com/fraunhoferfokus/open-content-decryption-module/milestones) page.

## How to build

### ...as Pepper Plugin for Chromium
* clone this repository, e.g. into $HOME/opencdm
* create the following symbolic link
 * ```$ ln -s $HOME/opencdm/src $CHROMIUM_ROOT/src/media/cdm/ppapi/external_open_cdm```
* add include into the ```$CHROMIUM_ROOT/src/media/media.gyp``` file to contain this:
```
  'includes': [
    'media_cdm.gypi',
    './cdm/ppapi/external_open_cdm/browser/media_open_cdm.gypi'
    ]
```
* apply changes by generating the project files
 * ```$ cd $CHROMIUM_ROOT/src```
 * ```$ build/gyp_chromium```
* build the following target (please follow *[Build notes for Chromium](docs/build_notes_chromium.md)* section before building)
 * ```$ ninja -C out/Debug opencdmadapter```

## How to run
* integrate OCDM with your browser
 * example to be found in src/browser folder
* setup communication to DRM system
 * sample code for this will be provided in the separate CDMi repository

In practice the Pepper Plugin API based OCDM implementation can be launched as follows:

```
./out/Debug/chrome --register-pepper-plugins="out/Debug/libopencdmadapter.so;application/x-ppapi-open-cdm"
```

## Folder Structure

Navigate the folders and see the readme files for further information.

## How to contribute

See the [wiki](https://github.com/fraunhoferfokus/open-content-decryption-module/wiki) for information on how to contribute to this project.

## Known Issues / Comments

This is a preliminary version of OCDM. Please file any issues or comments.

* Chromium sandbox: Currently Chromium needs to be started with the --no-sandbox flag because of the current RPC mechanism.
* Multiple session support is current work in progress.
* Code needs more review from the community (e.g. memory allocation, appropriate data types).

## License

Copyright 2014 Fraunhofer FOKUS

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
