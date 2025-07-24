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

#pragma once
#include <linux/gpio.h>
#include "gpio_1.h"

#include "core/simplefb.h"
#include "core/datatypes/forte_string.h"
#include "core/iec61131_functions.h"
#include "core/datatypes/forte_array_common.h"
#include "core/datatypes/forte_array.h"
#include "core/datatypes/forte_array_fixed.h"
#include "core/datatypes/forte_array_variable.h"

class FORTE_GPIO_controller final : public CSimpleFB {
  DECLARE_FIRMWARE_FB(FORTE_GPIO_controller)

  private:
    GPIO_Controller gpio;
    static const CStringDictionary::TStringId scmDataInputNames[];
    static const CStringDictionary::TStringId scmDataInputTypeIds[];
    static const TEventID scmEventREQID = 0;
    static const TDataIOID scmEIWith[];
    static const TForteInt16 scmEIWithIndexes[];
    static const CStringDictionary::TStringId scmEventInputNames[];
    static const TEventID scmEventCNFID = 0;
    static const TForteInt16 scmEOWithIndexes[];
    static const CStringDictionary::TStringId scmEventOutputNames[];

    static const SFBInterfaceSpec scmFBInterfaceSpec;

    CIEC_ANY *getVarInternal(size_t) override;

    void alg_REQ(void);

    void enterStateREQ(CEventChainExecutionThread *const paECET);

    void executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) override;

    void readInputData(TEventID paEIID) override;
    void writeOutputData(TEventID paEIID) override;
    void setInitialValues() override;

  public:
    FORTE_GPIO_controller(CStringDictionary::TStringId paInstanceNameId, forte::core::CFBContainer &paContainer);

    CIEC_STRING var_COMMAND;

    CEventConnection conn_CNF;

    CDataConnection *conn_COMMAND;

    CIEC_ANY *getDI(size_t) override;
    CIEC_ANY *getDO(size_t) override;
    CEventConnection *getEOConUnchecked(TPortId) override;
    CDataConnection **getDIConUnchecked(TPortId) override;
    CDataConnection *getDOConUnchecked(TPortId) override;

    void evt_REQ(const CIEC_STRING &paCOMMAND) {
      var_COMMAND = paCOMMAND;
      executeEvent(scmEventREQID, nullptr);
    }

    void operator()(const CIEC_STRING &paCOMMAND) {
      evt_REQ(paCOMMAND);
    }
};

