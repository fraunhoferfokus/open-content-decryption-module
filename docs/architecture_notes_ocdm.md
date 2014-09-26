# Architecture notes OCDM

As described in the main [README.md](../README.md) file, the scope of OCDM includes the CDM itself and the communication interfaces for CDM and media engine.

![Scope of OCDM](hhttps://raw.githubusercontent.com/fraunhoferfokus/open-content-decryption-module/master/docs/img/ocdm_scope.png "Scope of OCDM")

In order to support multiple browser environments and multiple DRM platforms at the same time the architecture of the OCDM has been designed using three different abstraction layers. This is described in the [3-layer architecture section](#3-layer architecture).

The architecture can be extended with a CDMi component to allow for an interoperable DRM architecture on platform level as well. There is also a corresponding OCDMi GitHub project available. How OCDM can be used with together with the OCDMi and CDMi compatible DRM systems is described in [CDM and CDMi](#CDM and CDMi).

## <a name="3-layer architecture"></a>3-layer architecture

Besides the division into the two components, CDM and media engine, OCDM is divided into three logical layers: browser glue, core and communication. The OCDM implementation is strictly bound to these layers by specific interfaces, which enables flexible exchange of a specific layer implementation with another one.

![OCDM 3-layer architecture](hhttps://raw.githubusercontent.com/fraunhoferfokus/open-content-decryption-module/master/docs/img/arch_layer_approach.png "OCDM 3-layer architecture")

Within the repository the browser glue code can be found at [src/browser/](../src/browser/). This glue code is used to talk to the component implementations of CDM and media engine. It dependends on the browser-specific EME implementation and might also be the responsible to link the CDM and  media engine instance. A PPAPI specific glue implementation for Chromium can be found in the OCDM repositoy at [src/browser/chrome/](../src/browser/chrome/). This implements the Chrome-defined CDM interface and uses the OCDM core CDM component to forward the calls to a proper communication mechanism.
Implementations for other browser environments just need to implement the EME-based interfaces according to their environment and use the core components accordingly.

The core component implementations are located at [src/cdm/](../src/cdm/) resp. [src/mediaengine/](../src/mediaengine/). Both are implemented to mediate the calls from the browser layer via the communication channel to the DRM platform. If no modifications are necessary to be done at this stage of processing the components can be used and instantiated as is. If a DRM system needs specific modifications the core components can either be exchanged or extended according to the needs.

The communication mechanisms are encapsulated by a communication interface and placed in [src/com/](../src/com/) for CDM [src/com/cdm/](../src/com/cdm/) and resp. [src/com/mediaengine/](../src/com/mediaengine/). The current OCDM implementation currently supports Unix RPC and shared memory. Other communication implementations can easily be added by using the communication interface without changes in the core layer and browser glue layer.

## <a name="CDM and CDMi"></a>CDM and CDMi

Content Decryption Module interface (CDMi) is an open specification that encapsulates DRM platform specific calls with a common interface. As this is an open specified interface, CDM implementations can use it to communicate with any DRM system that exposes a CDMi.

In addition to the OCDM repository we published a [OCDMi implementation](https://github.com/fraunhoferfokus/open-content-decryption-module-cdmi). The OCDMi can be used to implement an own CDMi-flavoured DRM platform service. It also has a mock implementation that works out of the box in combination with OCDM to demonstrate the usage and encourage own implemenations.

![CDM and CDMi](hhttps://raw.githubusercontent.com/fraunhoferfokus/open-content-decryption-module/master/docs/img/arch_cdm_cdmi.png "CDM and CDMi")

The OCDMi implementation is designed, similar to the OCDM, using a 3-layer approach but upside down. The communication layer holds CDMi core components that mediate calls to the DRM system glue code layer. For further details please refer to the [OCDMi repository](https://github.com/fraunhoferfokus/open-content-decryption-module-cdmi).

As shown in the figure above the connection between OCDM and OCDMi works using the communication mechanism in both components. The RPC mechanism used in OCDM is compatible with the RPC interface used by OCDMi. To change the communication mechanism the only change that needs to be done is creating an addditional implementation in the communication layer for both components.