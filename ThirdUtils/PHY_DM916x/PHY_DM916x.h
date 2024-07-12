
#ifndef __PHY_DM916X_H
#define __PHY_DM916X_H

#include <stdint.h>

/* Register Map */
#define DM916x_REG_BMCR              0x00U     /* Basic Control                      */
#define DM916x_REG_BMSR              0x01U     /* Basic Status                       */
#define DM916x_REG_PHYIDR1           0x02U     /* PHY Identifier 1                   */
#define DM916x_REG_PHYIDR2           0x03U     /* PHY Identifier 2                   */
#define DM916x_REG_ANAR              0x04U     /* Auto-Negotiation Advertisement     */
#define DM916x_REG_ANLPAR            0x05U     /* Auto-Neg. Link Partner Ability     */
#define DM916x_REG_ANER              0x06U     /* Auto-Neg. Expansion                */

/* PHY Extended Registers for DM9161/9162 */
#define DM916x_REG_DSCR        0x10     /* Specified Congfiguration Register            */
#define DM916x_REG_DSCSR       0x11     /* Specified Congfiguration and Status Register */
#define DM916x_REG_10BTCSR     0x12     /* 10Base-T Status/Control Register     		 */
#define DM916x_REG_PWDOR       0x13     /* Power Down Control Register       			 */
#define DM916x_REG_CONGFIG   	 0x14     /* Specified Congfig Register      			 */
#define DM916x_REG_INTERRUPT   0x15     /* Specified interrupt Register             	 */
#define DM916x_REG_SRECR       0x16     /* Specified Receive Error Counter  			 */
#define DM916x_REG_DISCR       0x17     /* Specified Disconnect Counter Register        */
#define DM916x_REG_RLSR        0x18     /* Hardware reset latch state Register			 */
#define DM916x_REG_PSCR        0x1D     /* Power Saving control register                */

/* Basic Control Register Bitmasks */
#define BMCR_RESET            0x8000U   /* Software Reset                     */
#define BMCR_LOOPBACK         0x4000U   /* Loopback mode                      */
#define BMCR_SPEED_SELECT     0x2000U   /* Speed Select (1=100Mb/s)           */
#define BMCR_ANEG_EN          0x1000U   /* Auto Negotiation Enable            */
#define BMCR_POWER_DOWN       0x0800U   /* Power Down                         */
#define BMCR_ISOLATE          0x0400U   /* Isolate Media interface            */
#define BMCR_RESTART_ANEG     0x0200U   /* Restart Auto Negotiation           */
#define BMCR_DUPLEX_MODE      0x0100U   /* Duplex Mode (1=Full duplex)        */
#define BMCR_COLLISION_TEST   0x0080U   /* Collision Test                     */

/* Basic Status Register Bitmasks */
#define BMSR_100B_T4          0x8000U   /* 100BASE-T4 Capable                 */
#define BMSR_100B_TX_FD       0x4000U   /* 100BASE-TX Full Duplex Capable     */
#define BMSR_100B_TX_HD       0x2000U   /* 100BASE-TX Half Duplex Capable     */
#define BMSR_10B_T_FD         0x1000U   /* 10BASE-T Full Duplex Capable       */
#define BMSR_10B_T_HD         0x0800U   /* 10BASE-T Half Duplex Capable       */
#define BMSR_NO_PREAMBLE      0x0040U   /* Preamble suppression               */
#define BMSR_ANEG_COMPLETE    0x0020U   /* Auto Negotiation Complete          */
#define BMSR_REMOTE_FAULT     0x0010U   /* Remote Fault                       */
#define BMSR_ANEG_ABILITY     0x0008U   /* Auto Negotiation Ability           */
#define BMSR_LINK_STAT        0x0004U   /* Link Status (1=link is up)         */
#define BMSR_JABBER_DETECT    0x0002U   /* Jabber Detect                      */
#define BMSR_EXT_CAPAB        0x0001U   /* Extended Capability                */

//#define PHY_ID_DM9161     0x0181B8B0  /* DM9161 PHY Identifier            */
//#define PHY_ID_DM9162     0x0181B8A0  /* DM9162 PHY Identifier            */
/* PHY Identifier Registers */
#define PHY_ID1              0x0181     /* DM916x Device Identifier MSB    */
#define PHY_ID2              0xB800     /* DM916x Device Identifier LSB    */

/* Auto-Negotiation Advertisement Register Bitmasks */
#define ANAR_NEXT_PAGE        0x8000U   /* Next page capable                  */
#define ANAR_REMOTE_FAULT     0x2000U   /* Remote fault supported             */
#define ANAR_PAUSE            0x0C00U   /* Pause                              */
#define ANAR_100B_T4          0x0200U   /* 100Base-T4 capable                 */
#define ANAR_100B_TX_FD       0x0100U   /* 100MBps full-duplex capable        */
#define ANAR_100B_TX_HD       0x0080U   /* 100MBps half-duplex capable        */
#define ANAR_10B_TX_FD        0x0040U   /* 10MBps full-duplex capable         */
#define ANAR_10B_TX_HD        0x0020U   /* 10MBps half-duplex capable         */
#define ANAR_SELECTOR_FIELD   0x001FU   /* Selector field (0x01==IEEE 802.3)  */

/* Auto-Negotiation Link Partner Ability Register Bitmasks */
#define ANLPAR_NEXT_PAGE      0x8000U   /* Next page capable                  */
#define ANLPAR_ACKNOWLEDGE    0x4000U   /* Acknowledge from partner           */
#define ANLPAR_REMOTE_FAULT   0x2000U   /* Remote fault detected              */
#define ANLPAR_PAUSE          0x0C00U   /* Pause                              */
#define ANLPAR_100B_TX_FD     0x0100U   /* 100MBps full-duplex capable        */
#define ANLPAR_100B_TX_HD     0x0080U   /* 100MBps half-duplex capable        */
#define ANLPAR_10B_TX_FD      0x0040U   /* 10MBps full-duplex capable         */
#define ANLPAR_10B_TX_HD      0x0020U   /* 10MBps half-duplex capable         */
#define ANLPAR_SELECTOR_FIELD 0x001FU   /* Selector field (0x01==IEEE 802.3)  */

/* Auto-Negotiation Expansion Register Bitmasks */
#define ANER_PDF              0x0010U   /* Parallel Detection Fault           */
#define ANER_LPAR_NEXT_PAGE   0x0008U   /* Link Partner Next Page Able        */
#define ANER_NEXT_PAGE        0x0004U   /* Next Page Able                     */
#define ANER_PAGE_RECEIVED    0x0002U   /* Page Received                      */
#define ANER_LPAR_ANEG        0x0001U   /* Link Partner Auto-Negotiation Able */


/* Specified Congfiguration and Status Register */
#define DSCSR_100M_FD         0x8000U   /* 100Base-TX Full-duplex bitmask    */
#define DSCSR_100M_HD         0x4000U   /* 100Base-TX Half-duplex bitmask    */
#define DSCSR_10M_FD          0x2000U   /* 10Base-TX Full-duplex bitmask     */
#define DSCSR_10M_HD          0x1000U   /* 10Base-TX Half-duplex bitmask     */

/* PHY Driver State Flags */
#define PHY_INIT              0x01U     /* Driver initialized                */
#define PHY_POWER             0x02U     /* Driver power is on                */

#define  DM916x_STATUS_READ_ERROR            ((int32_t)-5)
#define  DM916x_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  DM916x_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  DM916x_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  DM916x_STATUS_ERROR                 ((int32_t)-1)
#define  DM916x_STATUS_OK                    ((int32_t) 0)
#define  DM916x_STATUS_LINK_DOWN             ((int32_t) 1)
#define  DM916x_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  DM916x_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  DM916x_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  DM916x_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  DM916x_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)

typedef int32_t  (*dm916x_Init_Func) (void); 
typedef int32_t  (*dm916x_DeInit_Func) (void);
typedef int32_t  (*dm916x_ReadReg_Func)   (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*dm916x_WriteReg_Func)  (uint32_t, uint32_t, uint32_t);
typedef int32_t  (*dm916x_GetTick_Func)  (void);

typedef struct 
{                   
  dm916x_Init_Func      Init; 
  dm916x_DeInit_Func    DeInit;
  dm916x_WriteReg_Func  WriteReg;
  dm916x_ReadReg_Func   ReadReg; 
  dm916x_GetTick_Func   GetTick;   
} dm916x_IOCtx_t;  

  
typedef struct 
{
  uint32_t            DevAddr;
  uint32_t            Is_Initialized;
  dm916x_IOCtx_t     IO;
  void               *pData;
} dm916x_Object_t;

int32_t DM916x_RegisterBusIO(dm916x_Object_t *pObj, dm916x_IOCtx_t *ioctx);
int32_t DM916x_Init(dm916x_Object_t *pObj);
int32_t DM916x_GetLinkState(dm916x_Object_t *pObj);

#endif /* __PHY_DM916x_H */
