/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x NG!
 ***
 *** Name: GPIO_controller
 *** Description: Simple FB with one algorithm
 *** Version:
 ***     1.0: 2025-07-17/felix -  -
 *************************************************************************/

#include <linux/gpio.h>
#include "gpio_1.h"
#include "gpiocontroller_fbt.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "gpiocontroller_fbt_gen.cpp"
#endif

#include "core/iec61131_functions.h"
#include "core/datatypes/forte_array_common.h"
#include "core/datatypes/forte_array.h"
#include "core/datatypes/forte_array_fixed.h"
#include "core/datatypes/forte_array_variable.h"

#include <string>
#include <iostream>
static GPIO_Controller gpio;

DEFINE_FIRMWARE_FB(FORTE_GPIO_controller, g_nStringIdGPIO_controller)

const CStringDictionary::TStringId FORTE_GPIO_controller::scmDataInputNames[] = {g_nStringIdCOMMAND};
const CStringDictionary::TStringId FORTE_GPIO_controller::scmDataInputTypeIds[] = {g_nStringIdSTRING};
const TDataIOID FORTE_GPIO_controller::scmEIWith[] = {0, scmWithListDelimiter};
const TForteInt16 FORTE_GPIO_controller::scmEIWithIndexes[] = {0};
const CStringDictionary::TStringId FORTE_GPIO_controller::scmEventInputNames[] = {g_nStringIdREQ};
const TForteInt16 FORTE_GPIO_controller::scmEOWithIndexes[] = {-1};
const CStringDictionary::TStringId FORTE_GPIO_controller::scmEventOutputNames[] = {g_nStringIdCNF};
const SFBInterfaceSpec FORTE_GPIO_controller::scmFBInterfaceSpec = {
  1, scmEventInputNames, nullptr, scmEIWith, scmEIWithIndexes,
  1, scmEventOutputNames, nullptr, nullptr, scmEOWithIndexes,
  1, scmDataInputNames, scmDataInputTypeIds,
  0, nullptr, nullptr,
  0, nullptr,
  0, nullptr
};

FORTE_GPIO_controller::FORTE_GPIO_controller(const CStringDictionary::TStringId paInstanceNameId, forte::core::CFBContainer &paContainer) :
    CSimpleFB(paContainer, scmFBInterfaceSpec, paInstanceNameId, nullptr),
    var_COMMAND(""_STRING),
    conn_CNF(this, 0),
    conn_COMMAND(nullptr) {
}

void FORTE_GPIO_controller::setInitialValues() {
  var_COMMAND = ""_STRING;
}

void FORTE_GPIO_controller::executeEvent(const TEventID paEIID, CEventChainExecutionThread *const paECET) {
  switch(paEIID) {
    case scmEventREQID:
      enterStateREQ(paECET);
      break;
    default:
      break;
  }
}

void FORTE_GPIO_controller::enterStateREQ(CEventChainExecutionThread *const paECET) {
  alg_REQ();
  sendOutputEvent(scmEventCNFID, paECET);
}

void FORTE_GPIO_controller::readInputData(const TEventID paEIID) {
  switch(paEIID) {
    case scmEventREQID: {
      readData(0, var_COMMAND, conn_COMMAND);
      break;
    }
    default:
      break;
  }
}

void FORTE_GPIO_controller::writeOutputData(TEventID) {
  // nothing to do
}

CIEC_ANY *FORTE_GPIO_controller::getDI(const size_t paIndex) {
  switch(paIndex) {
    case 0: return &var_COMMAND;
  }
  return nullptr;
}

CIEC_ANY *FORTE_GPIO_controller::getDO(size_t) {
  return nullptr;
}

CEventConnection *FORTE_GPIO_controller::getEOConUnchecked(const TPortId paIndex) {
  switch(paIndex) {
    case 0: return &conn_CNF;
  }
  return nullptr;
}

CDataConnection **FORTE_GPIO_controller::getDIConUnchecked(const TPortId paIndex) {
  switch(paIndex) {
    case 0: return &conn_COMMAND;
  }
  return nullptr;
}

CDataConnection *FORTE_GPIO_controller::getDOConUnchecked(TPortId) {
  return nullptr;
}

CIEC_ANY *FORTE_GPIO_controller::getVarInternal(size_t) {
  return nullptr;
}

void FORTE_GPIO_controller::alg_REQ(void) {
  // Получаем строку команды
  std::string cmd = std::string(var_COMMAND.getValue());

  // Выполняем соответствующее действие
  if(cmd == "forward") {
    gpio.forward(200);
  } else if(cmd == "stop") {
    gpio.stop();
  } else if(cmd == "left") {
    gpio.left(100);
  } else if(cmd == "right") {
    gpio.right(100);
  } else {
    // Неизвестная команда
    std::cerr << "Unknown command: " << cmd << std::endl;
  }
}
