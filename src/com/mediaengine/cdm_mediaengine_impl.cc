/*
* Copyright (C) 2014, Fraunhofer Institute for Open Communication Systems FOKUS
*/
#include "./cdm_mediaengine_impl.h"
#include <iostream>  // TODO(sph): remove *stream when not needed anymore
#include <fstream>
#include "../common/rpc/cdmxdr.h"


namespace cdm {

  std::string MediaEngineSession::rpcServer = "localhost";

  MediaEngineSession::MediaEngineSession(
      uint16_t *session_id_val,
      uint32_t session_id_len) {
    std::cout<< "MediaEngineSession::MediaEngineSession" <<std::endl;


    sessionId.id = new uint16_t[session_id_len];
    memcpy(sessionId.id, session_id_val, session_id_len);
    sessionId.id = session_id_val;
    sessionId.idLen = session_id_len;

    if ((rpcClient = clnt_create(rpcServer.c_str(), CDM_PROG, CDM_VERS, "tcp"))
     == NULL) {
       clnt_pcreateerror(rpcServer.c_str());
      std::cout << "rpcclient creation failed: " << rpcServer.c_str();
    } else {
      std::cout << "MediaEngineSession connected to server" <<std::endl;
    }

    // CREATE MediaEngineSession
    mes_response *response_mediaEngineSession;

    me_data param_mediaEngineSession;
    param_mediaEngineSession.session_id.session_id_val = sessionId.id;
    param_mediaEngineSession.session_id.session_id_len = sessionId.idLen;


    // TODO(ska): need to check == 0?
    // if (idXchngShMem == 0) {
      idXchngShMem = AllocateSharedMemory(sizeof(shmem_info));
      if (idXchngShMem < 0) {
        std::cout << "idXchngShMem AllocateSharedMemory failed!" << std::endl;
      }
    // }
    shMemInfo = reinterpret_cast<shmem_info *>(MapSharedMemory(idXchngShMem));

    // SHARED MEMORY and SEMAPHORE INITIALIZATION
    shMemInfo->idSidShMem = 0;
    this->shMemInfo->idIvShMem = 0;
    this->shMemInfo->idSampleShMem = 0;
    this->shMemInfo->idSubsampleDataShMem = 0;
    this->shMemInfo->sidSize = 1;
    this->shMemInfo->ivSize = 2;
    this->shMemInfo->sampleSize = 3;
    this->shMemInfo->subsampleDataSize = 4;

    unsigned short vals[3];
    vals[SEM_XCHNG_PUSH] = 1;
    vals[SEM_XCHNG_DECRYPT] = 0;
    vals[SEM_XCHNG_PULL] = 0;
    // if (idXchngSem == 0) {
      idXchngSem = CreateSemaphoreSet(3, vals);
      if (idXchngSem < 0) {
        std::cout << "idXchngSem CreateSemaphoreSet failed!" << std::endl;
      }
    // }


    param_mediaEngineSession.idXchngShMem = idXchngShMem;
    param_mediaEngineSession.idXchngSem = idXchngSem;

    if ((response_mediaEngineSession = cdm_mediaenginesession_rpc_1
      (&param_mediaEngineSession, rpcClient)) == NULL) {
      std::cout << "engine session failed: " << rpcServer.c_str();
       clnt_perror(rpcClient, rpcServer.c_str());
       exit(5);
    } else {
      std::cout << "engine session creation called" << std::endl;
    }


    std::cout << "create media engine session platform response: " <<
        response_mediaEngineSession->return_val <<std::endl;
  }

  MediaEngineSession::~MediaEngineSession() {
    std::cout << "MediaEngineSession destruct!";
    // TODO(ska): is shared memory cleaned up correctly?
    // DeleteSemaphoreSet(idXchngSem);
    idXchngSem = 0;
    idXchngShMem = 0;
  }

  DecryptResponse MediaEngineSession::Decrypt(uint8_t *pbIv, uint32_t cbIv,
    uint8_t *pbData, uint32_t cbData, uint8_t *out, uint32_t &out_size) {
    std::cout << "MediaEngineSession::Decrypt: "<< std::endl;
    DecryptResponse response;
    response.platform_response = PLATFORM_CALL_SUCCESS;
    response.sys_err = 0;  // TODO(sph): real decryptresponse values

    std::cout << "pbData" <<std::endl;
    std::cout << std::hex
        << (int) pbData[0] << " " << (int) pbData[1] << " "
        << (int) pbData[2] << " "  << (int) pbData[3] << " "
        << (int) pbData[4] << " "  << (int) pbData[5] << " "
        << (int) pbData[6] << " "  << (int) pbData[7]
        << std::dec << std::endl;

    LockSemaphore(idXchngSem, SEM_XCHNG_PUSH);
    std::cout << "LOCKed push lock" << std::endl;

    cbIv = (cbIv != 8) ? 8 : cbIv;
    shMemInfo->idIvShMem = AllocateSharedMemory(cbIv);
    shMemInfo->ivSize = cbIv;

    uint8_t *pIvShMem = reinterpret_cast<uint8_t *>(MapSharedMemory
      (shMemInfo->idIvShMem));
    memcpy(pIvShMem, pbIv, cbIv);
    // delete[] pbIv;

    shMemInfo->idSampleShMem = AllocateSharedMemory(cbData);
    shMemInfo->sampleSize = cbData;
    uint8_t *pSampleShMem = reinterpret_cast<uint8_t *>(MapSharedMemory
      (shMemInfo->idSampleShMem));

    memcpy(pSampleShMem, pbData, cbData);
    // delete[] pbData;

    shMemInfo->idSubsampleDataShMem = 0;
    shMemInfo->subsampleDataSize = 0;
    std::cout << "data ready to decrypt" << std::endl;
    UnlockSemaphore(idXchngSem, SEM_XCHNG_DECRYPT);
    std::cout << "WAIT for pull lock" << std::endl;
    LockSemaphore(idXchngSem, SEM_XCHNG_PULL);
    std::cout << "LOCKed pull lock" << std::endl;
    // process clear data
    std::cout << std::hex
        << (int) pSampleShMem[0] << " " << (int) pSampleShMem[1] << " "
        << (int) pSampleShMem[2] << " "  << (int) pSampleShMem[3] << " "
        << (int) pSampleShMem[4] << " "  << (int) pSampleShMem[5] << " "
        << (int) pSampleShMem[6] << " "  << (int) pSampleShMem[7]
        << std::dec << std::endl;

    // out = pSampleShMem;
    memcpy(out,pSampleShMem, cbData);
    out_size = cbData;

    std::cout << "out: " << out_size << std::endl;
    std::cout << std::hex
        << (int) out[0] << " " << (int) out[1] << " "  << (int) out[2]
         << " "  << (int) out[3] << " "
        << (int) out[4] << " "  << (int) out[5] << " "  << (int) out[6]
        << " "  << (int) out[7]
        << std::dec << std::endl;

    std::cout << "RUN fired!" << std::endl;
    UnlockSemaphore(idXchngSem, SEM_XCHNG_PUSH);
    std::cout << "UNLOCKed push lock" << std::endl;

    // clean up current shared mems for sample data
    int err = DetachExistingSharedMemory(pIvShMem);
    std::cout << "detached iv shmem " << shMemInfo->idIvShMem << ": "
    << err << std::endl;
    err = DetachExistingSharedMemory(pSampleShMem);
    std::cout << "detached sample shmem " << shMemInfo->idSampleShMem
     << ": " << err << std::endl;


    return response;
  }


  void MediaEngineSession::MediaKeySession_Close() {
    /*close*/
    params_close par_close;
    par_close.session_id.session_id_len = sessionId.idLen;
    par_close.session_id.session_id_val = sessionId.id;
    int *cresult;
    if ((cresult = close_rpc_1(&par_close, rpcClient)) == NULL) {
      clnt_perror(rpcClient, rpcServer.c_str());
      exit(10);
    }

    if (*cresult == 0) {
      std::cout << "MediaKeySession_Close success\n ";
    } else {
      std::cout << "MediaKeySession_Close failed\n ";
    }

    clnt_destroy(rpcClient);
  }

}  // namespace cdm
