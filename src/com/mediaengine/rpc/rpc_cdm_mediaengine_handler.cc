/*
 * Copyright (c) 2017 Fraunhofer FOKUS
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include "media/cdm/ppapi/external_open_cdm/com/mediaengine/rpc/rpc_cdm_mediaengine_handler.h"
#include "media/cdm/ppapi/cdm_logging.h"
#include "media/cdm/ppapi/external_open_cdm/com/common/rpc/opencdm_xdr.h"

namespace media {

std::string rpcServer = "localhost";
// TODO(ska): outsource to config

RpcCdmMediaengineHandler::RpcCdmMediaengineHandler(uint16_t *session_id_val,
                                                   uint32_t session_id_len,
                                                   uint8_t *auth_data_val,
                                                   uint32_t auth_data_len) {
  CDM_DLOG() << "RpcCdmMediaengineHandler::RpcCdmMediaengineHandler";

  sessionId.id = new uint16_t[session_id_len];
  memcpy(sessionId.id, session_id_val, session_id_len);
  // TODO(ska): do we need this memcpy?
  sessionId.id = session_id_val;
  sessionId.idLen = session_id_len;

  if ((rpcClient = clnt_create(rpcServer.c_str(), OPEN_CDM, OPEN_CDM_EME_5,
                               "tcp")) == NULL) {
    clnt_pcreateerror(rpcServer.c_str());
    CDM_DLOG() << "rpcclient creation failed: " << rpcServer.c_str();
  } else {
    CDM_DLOG() << "RpcCdmMediaengineHandler connected to server";
  }

  rpc_response_generic *rpc_response;
  rpc_request_mediaengine_data rpc_param;
  rpc_param.session_id.session_id_val = session_id_val;
  rpc_param.session_id.session_id_len = session_id_len;
  rpc_param.auth_data.auth_data_val = auth_data_val;
  rpc_param.auth_data.auth_data_len = auth_data_len;

  // TODO(ska): need to check == 0?
  // if (idXchngShMem == 0) {
  idXchngShMem = AllocateSharedMemory(sizeof(shmem_info));
  if (idXchngShMem < 0) {
    CDM_DLOG() << "idXchngShMem AllocateSharedMemory failed!";
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
    CDM_DLOG() << "idXchngSem CreateSemaphoreSet failed!";
  }
  // }

  rpc_param.id_exchange_shmem = idXchngShMem;
  rpc_param.id_exchange_sem = idXchngSem;

  if ((rpc_response = rpc_open_cdm_mediaengine_1(&rpc_param, rpcClient))
      == NULL) {
    CDM_DLOG() << "engine session failed: " << rpcServer.c_str();
    clnt_perror(rpcClient, rpcServer.c_str());
    exit(5);
  } else {
    CDM_DLOG() << "engine session creation called";
  }

  CDM_DLOG() << "create media engine session platform response: "
             << rpc_response->platform_val;
}

RpcCdmMediaengineHandler::~RpcCdmMediaengineHandler() {
  CDM_DLOG() << "RpcCdmMediaengineHandler destruct!";
  // TODO(ska): is shared memory cleaned up correctly?
  // DeleteSemaphoreSet(idXchngSem);
  idXchngSem = 0;
  idXchngShMem = 0;
}

DecryptResponse RpcCdmMediaengineHandler::Decrypt(const uint8_t *pbIv,
                                                  uint32_t cbIv,
                                                  const uint8_t *pbData,
                                                  uint32_t cbData, uint8_t *out,
                                                  uint32_t &out_size) {
  CDM_DLOG() << "RpcCdmMediaengineHandler::Decrypt: ";
  DecryptResponse response;
  response.platform_response = PLATFORM_CALL_SUCCESS;
  response.sys_err = 0;
  // TODO(sph): real decryptresponse values need to
  // be written to sharedmem as well

  LockSemaphore(idXchngSem, SEM_XCHNG_PUSH);
  CDM_DLOG() << "LOCKed push lock";

  cbIv = (cbIv != 8) ? 8 : cbIv;
  shMemInfo->idIvShMem = AllocateSharedMemory(cbIv);
  shMemInfo->ivSize = cbIv;

  uint8_t *pIvShMem = reinterpret_cast<uint8_t *>(MapSharedMemory(
      shMemInfo->idIvShMem));
  memcpy(pIvShMem, pbIv, cbIv);
  // delete[] pbIv;

  shMemInfo->idSampleShMem = AllocateSharedMemory(cbData);
  shMemInfo->sampleSize = cbData;
  uint8_t *pSampleShMem = reinterpret_cast<uint8_t *>(MapSharedMemory(
      shMemInfo->idSampleShMem));

  memcpy(pSampleShMem, pbData, cbData);
  // delete[] pbData;

  shMemInfo->idSubsampleDataShMem = 0;
  shMemInfo->subsampleDataSize = 0;
  CDM_DLOG() << "data ready to decrypt";
  UnlockSemaphore(idXchngSem, SEM_XCHNG_DECRYPT);
  CDM_DLOG() << "WAIT for pull lock";
  LockSemaphore(idXchngSem, SEM_XCHNG_PULL);
  CDM_DLOG() << "LOCKed pull lock";
  // process clear data

  memcpy(out, pSampleShMem, cbData);
  out_size = cbData;

  CDM_DLOG() << "RUN fired!";
  UnlockSemaphore(idXchngSem, SEM_XCHNG_PUSH);
  CDM_DLOG() << "UNLOCKed push lock";

  // clean up current shared mems for sample data
  int err = DetachExistingSharedMemory(pIvShMem);
  CDM_DLOG() << "detached iv shmem " << shMemInfo->idIvShMem << ": " << err;
  err = DetachExistingSharedMemory(pSampleShMem);
  CDM_DLOG() << "detached sample shmem " << shMemInfo->idSampleShMem << ": "
             << err;

  return response;
}


}  // namespace media
