/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/
#include "./cdm_platform_impl.h"
#include <iostream>
// TODO(sph): replace iostream with logging lib
#include "../common/rpc/cdmxdr.h"

namespace cdm {

  std::string CDMPlatform::rpcServer = "localhost";

  CDMPlatform::CDMPlatform() {
  }

  CDMPlatform::~CDMPlatform() {
  }

  PlatformResponse CDMPlatform::MediaKeys(std::string keySystem) {
    PlatformResponse response;

      if ((rpcClient = clnt_create(rpcServer.c_str(),
          CDM_PROG, CDM_VERS, "tcp")) == NULL) {
         clnt_pcreateerror(rpcServer.c_str());
      } else {
         std::cout<< "CDMPlatform connected to server" <<std::endl;
      }

      /*Cdm_MediaKeys*/
      int *result;
      const char *ks = keySystem.c_str();
      if ((result = cdm_mediakeys_rpc_1(const_cast<char **>(&ks),
        rpcClient)) == NULL) {
        clnt_perror(rpcClient, rpcServer.c_str());
      }

      if (*result == 0) {
        std::cout<< "cdm_mediakeys_rpc_1 success" <<std::endl;
        response.platform_response = PLATFORM_CALL_SUCCESS;
      } else {
        std::cout<< "cdm_mediakeys_rpc_1 failed" <<std::endl;
        response.platform_response = PLATFORM_CALL_FAIL;
      }

    return response;
  }

  PlatformResponse CDMPlatform::MediaKeys_CreateSession(std::string mimeType,
      const uint8_t *pbInitData, uint32_t cbInitData,
      uint8_t *pbCDMData, uint32_t cbCDMData) {
    PlatformResponse response;

    /*createSession*/
    session_response *csresult;
    session sessionmessage;
    // TODO(ska): rename session to something more expressive

    uint32_t temp_f_cbInitData = (uint32_t) cbInitData;
    uint8_t *temp_f_pbInitData =
      reinterpret_cast<uint8_t *>(malloc(temp_f_cbInitData * sizeof(uint8_t)));
    memcpy(temp_f_pbInitData, pbInitData, temp_f_cbInitData);

    sessionmessage.type = const_cast<char *>(mimeType.c_str());
    sessionmessage.pbInitData.pbInitData_val = temp_f_pbInitData;
    sessionmessage.pbInitData.pbInitData_len = temp_f_cbInitData;

    uint32_t temp_f_cbCDMData = 1;
    uint8_t *temp_f_pbCDMData = reinterpret_cast<uint8_t *>(
      malloc(temp_f_cbCDMData));
    sessionmessage.pbCDMData.pbCDMData_len = temp_f_cbCDMData;
    sessionmessage.pbCDMData.pbCDMData_val = temp_f_pbCDMData;

    // initialize sessionmessage
    if ((csresult =
        createsession_rpc_1(&sessionmessage, rpcClient)) == NULL) {
      clnt_perror(rpcClient, rpcServer.c_str());
      exit(6);
    }

    if (csresult->return_val == 0) {
      response.platform_response = PLATFORM_CALL_SUCCESS;
      m_session_id_len = csresult->session_id.session_id_len;
      m_session_id_val = csresult->session_id.session_id_val;
    } else {
      response.platform_response = PLATFORM_CALL_FAIL;
    }

    free(temp_f_pbInitData);
    free(temp_f_pbCDMData);

    return response;
  }

  PlatformResponse CDMPlatform::MediaKeys_CreateSession(std::string mimeType,
      const uint8_t *pbInitData, uint32_t cbInitData) {
    return MediaKeys_CreateSession(mimeType, pbInitData, cbInitData, 0, 0);
  }

  PlatformResponse CDMPlatform::MediaKeySession_Run(
      uint16_t *session_id_val, uint32_t session_id_len) {
    PlatformResponse response;

    params_run prun;
    prun.session_id.session_id_len = session_id_len;
    prun.session_id.session_id_val = session_id_val;

    int *runresult;
    if ((runresult = run_rpc_1(&prun, rpcClient)) == NULL) {
      clnt_perror(rpcClient, rpcServer.c_str());
    }

    if (*runresult == 0) {
      response.platform_response = PLATFORM_CALL_SUCCESS;
    } else {
      response.platform_response = PLATFORM_CALL_FAIL;
    }

    return response;
  }

  PlatformResponse CDMPlatform::MediaKeySession_Update(
      const uint8_t *pbKey, uint32_t cbKey,
      uint16_t *session_id_val, uint32_t session_id_len) {
    PlatformResponse response;

    /*update*/
    int *uresult;

    params_update pupdate;

    uint32_t temp_f_cbKey = (uint32_t) cbKey;
    uint8_t *temp_f_pbKey =
      reinterpret_cast<uint8_t *>(malloc(temp_f_cbKey * sizeof(uint8_t)));
    memcpy(temp_f_pbKey, pbKey, temp_f_cbKey);

    pupdate.session_id.session_id_val = session_id_val;
    pupdate.session_id.session_id_len = session_id_len;
    pupdate.challenge_response.challenge_response_val = temp_f_pbKey;
    pupdate.challenge_response.challenge_response_len = temp_f_cbKey;

    if ((uresult = update_rpc_1(&pupdate, rpcClient)) == NULL) {
      clnt_perror(rpcClient, rpcServer.c_str());
      exit(7);
    }

    if (*uresult == 0) {
      response.platform_response = PLATFORM_CALL_SUCCESS;
    } else {
      response.platform_response = PLATFORM_CALL_FAIL;
    }

    return response;
  }

  PlatformResponse CDMPlatform::MediaKeySession_Close(
      uint16_t *session_id_val, uint32_t session_id_len) {
    PlatformResponse response;

    /*close*/
    params_close par_close;
    par_close.session_id.session_id_len = session_id_len;
    par_close.session_id.session_id_val = session_id_val;
    int *cresult;
    if ((cresult = close_rpc_1(&par_close, rpcClient)) == NULL) {
      clnt_perror(rpcClient, rpcServer.c_str());
      exit(10);
    }

    if (*cresult == 0) {
      response.platform_response = PLATFORM_CALL_SUCCESS;
    } else {
      response.platform_response = PLATFORM_CALL_FAIL;
    }

    clnt_destroy(rpcClient);
    return response;
  }

  uint32_t  CDMPlatform::SessionIdLength() {
    return m_session_id_len;
  }

  uint16_t* CDMPlatform::SessionIdValue() {
    return m_session_id_val;
  }

}  // namespace cdm
