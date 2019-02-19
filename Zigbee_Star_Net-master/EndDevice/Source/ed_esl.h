/*
 * ed_esl.h
 *
 *  Created on: 2017年10月6日
 *      Author: xubov
 */

#ifndef ED_ESL_H_
#define ED_ESL_H_

#include <jendefs.h>
#include "node.h"
/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void printfNode_sData(NODE_tsData *NODE_sData);
PUBLIC void esl_Init(void);
PUBLIC void esl_vHwIndHandle(AppQApiHwInd_s *psAHI_Ind);
PUBLIC void ed_esl_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd);
PUBLIC void ed_esl_usrPoll(void);
PUBLIC void ed_vIndAssociate(MAC_MlmeDcfmInd_s *psMlmeInd);
PUBLIC uint8 getActiveCh(uint8 num);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* ED_ESL_H_ */
