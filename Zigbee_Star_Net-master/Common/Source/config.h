/*****************************************************************************/
/*!
 *\MODULE              Config (Application Layer)
 *
 *\COMPONENT           $HeadURL: https://www.collabnet.nxp.com/svn/lprf_apps/Application_Notes/JN-AN-1069-IEEE-802.15.4-Serial-Cable-Replacement/Trunk/Common/Source/config.h $
 *
 *\VERSION			   $Revision: 16555 $
 *
 *\REVISION            $Id: config.h 16555 2015-11-20 12:14:37Z nxa04494 $
 *
 *\DATED               $Date: 2015-11-20 12:14:37 +0000 (Fri, 20 Nov 2015) $
 *
 *\AUTHOR              $Author: nxa04494 $
 *
 *\DESCRIPTION         Config (Definitions)
 */
/****************************************************************************
 *
 * This software is owned by Jennic and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on Jennic products. You, and any third parties must reproduce
 * the copyright and warranty notice and any other legend of ownership on each
 * copy or partial copy of the software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
 * BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
 *
 * Copyright Jennic Ltd 2009. All rights reserved
 *
 ****************************************************************************/

#ifndef  CONFIG_H_INCLUDED
#define  CONFIG_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* software version */
#define VERSION                     "\r\n************ESL_JN5168_SLAVE_V0.0.1************\r\n"
#define ESL_DEV                     "\r\n************        ESL_DEV        ************\r\n"
#define CRD_DEV                     "\r\n************        CRD_DEV        ************\r\n"
#define PAN_DEV                     "\r\n************        PAN_DEV        ************\r\n"

/* application */
#define ESL                         1
//#define END_DEV_SIMULATE            1 /* simulator 300nodes to try to joined network */
//#define REMOTE_CTRL                 2

/* Device Type */
#define FFD                         0x02 //MAC_MlmeIndAssociate_s.u8Capability[Device Type]. JN-UG-3024 v2.6 page 130
#define RFD                         0x00

#define END_DEVICE                  RFD
#define CRD_DEVICE                  FFD

#define LED_ON                      TRUE
#define LED_OFF                     FALSE

/* Network parameters */
#define PAN_ID                      0x1707
#define PAN_ID_BAK                  0x1708
#define MAX_CHILDREN             	340

#define MAX_ROUTER 5
#define MAX_DEEPTH 3
#define MAX_CHLDR  300


/* Defines the channels to scan. Each bit represents one channel. All channels
   in the channels (11-26) in the 2.4GHz band are scanned. */
//#define SCAN_CHANNELS 		    0x07FFF800UL
#define SCAN_CHANNELS 		        0x02108000UL//0x02108000UL //15, 20, 25
#define SCAN_CH_NUM                 3
#define SCAN_1ST_CH                 (15)
#define SCAN_2ND_CH                 (20)
#define SCAN_3RD_CH                 (25)
#define CHANNEL_MIN                 11

/* Sleep period between scans of the full channel mask for end devices */
#define SCAN_SLEEP					32000

/* Sleep Time 32000 = 1s; 1ms = 32 */
#define END_SCAN_SLEEP_SECOND       (100)
#define END_SCAN_SLEEP				(32000*END_SCAN_SLEEP_SECOND)	//1s * N
#define END_NORMAL_SLEEP			(32000*100)	//1s * N
#define CRD_NORMAL_SLEEP			(uint64)(32000*2.5) //1s * 3
//#define NORMAL_SLEEP			    (32000*1) //1s * 3
#define NORMAL_SLEEP			    (16000*1) //0.5

/* Keep wakeup time*/
#define KEEP_AWAKE_SHORT_TIMER      0    // * 10ms
#define KEEP_AWAKE_LONG_TIMER       1000 // * 10ms
#define KEEP_AWAKE_1S               100  // * 10ms

/* Ticks per ms on tick timer */
#define TICK_TIMER_MS  				16000

#define ACTIVE_SCAN_TIMES           40
/* Duration (ms) = 15.36ms x (2^ACTIVE_SCAN_DURATION + 1) */
#define ACTIVE_SCAN_DURATION        3
/* Duration (ms) = 15.36ms x (2^ENERGY_SCAN_DURATION + 1) */
#define ENERGY_SCAN_DURATION        3

/***********************************************/
/* pdm_id */
// eeprom£º seg[0..63]
#define MAX_SECTION 60
#define MIN_SECTION 0

#define PDM_ID_APP_COORD        0
#define PDM_ID_APP_ROUTER       1
#define PDM_ID_APP_END          2
#define PDM_ID_NODE_PARM        3
#define CHILD_TABLE_MIN_SEG     5
#define CHILD_TABLE_MAX_SEG     60

#define CHILD_TABLE_SEGMENT_LEN 64
#define CHILD_TABLE_SEGMENT_STATUS_OFFSET 60
#define CHILD_TABLE_SEGMENT_TABLE_START 0
#define CHILD_TABLE_CHD_NUM_PER_SEG     6
/***********************************************/

#ifdef NULL
	#undef NULL
	#define  NULL   0x00u
#else
	#define NULL 0x00u
#endif


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* CONFIG_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
