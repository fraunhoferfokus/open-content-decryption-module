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

## Scope

OCDM is developed according to W3C EME. This leads to a architecture as shown in the figure below, which consists of a Web application, a browser and a DRM platform layer. The browser as mediator between Web application and DRM platform exposes the EME and contains a CDM. The Web application is connected to the browser via EME. EME is mapped by the browser to a CDM and the CDM communicates to the DRM platform.

As EME is modelling only the license retrieval and management, there is a second component that needs to talk to the DRM platform. In many cases, mainly in embedded environments, the media engine is separate from the browser and used by the browser as external renderer. In this case the media engine probably needs to authenticate itself at the DRM platform for handling and decoding of protected media data.

![Scope of OCDM](https://raw.githubusercontent.com/fraunhoferfokus/open-content-decryption-module/master/docs/img/ocdm_scope.png "Scope of OCDM")

On the one hand, the OCDM's scope includes the generic CDM and its communication interfaces to the DRM platform. On the other hand, there are separate media engine communication interfaces to the DRM platform. OCDM contains sample implementations for the RPC communication mechanism, but is designed highly modular to be easily extended with other communication mechanisms.

For detailed internals of the OCDM architecture please refer to the [architecture notes](./docs/architecture_notes_ocdm.md).

## References

* [1] W3C Encrypted Media Extensions https://w3c.github.io/encrypted-media
* [2] Interoperability Digital Rights Management and the Web https://scrivito-public.s3-eu-west-1.amazonaws.com/fokus/64257f12b90b464b/64a4d370cf46918585d0276caa858eae/FAME_Interoperability_Digital_Rights_Management_and_the_Web_flyer.pdf
* [3] MSE-EME Reference Platform Documentation https://html5.cablelabs.com/mse-eme/doc/overview.html
* [4] Content Decryption Module Interface Specification http://download.microsoft.com/download/E/A/4/EA470677-6C3C-4AFE-8A86-A196ADFD0F78/Content%20Decryption%20Module%20Interface%20Specification.pdf
* [5] Fraunhofer FOKUS FAMIUM DRM http://www.fokus.fraunhofer.de/go/drm

## Supported Browsers and Platforms

Currently OCDM development is compatible with following Web browers:

* Chromium / CEF
 * Linux
* Opera SDK
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
 * sample code for this is provided in the separate [Open Content Decryption Module CDMi](https://github.com/fraunhoferfokus/open-content-decryption-module-cdmi) repository

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

For more details see the [milestones](https://github.com/fraunhoferfokus/open-content-decryption-module/milestones) page.
