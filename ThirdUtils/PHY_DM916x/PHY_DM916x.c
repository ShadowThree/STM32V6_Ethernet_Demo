#include "PHY_DM916x.h"

#define PHY_DBG_EN	1
#if PHY_DBG_EN
#include "dbger.h"
#define PHY_DBG		LOG_DBG
#define PHY_ERR		LOG_ERR
#else
#define PHY_DBG
#define PHY_ERR
#endif

#define DM916x_SW_RESET_TO    ((uint32_t)500U)
#define DM916x_INIT_TO        ((uint32_t)2000U)
#define DM916x_MAX_DEV_ADDR   ((uint32_t)31U)

#define DM916x_LINK_STAT_DOWN		1
#define DM916x_LINK_STAT_UP			2
volatile uint8_t is_dm916x_link_up = 0;
/**
  * @brief  Initialize the dm916x and configure the needed hardware resources
  * @param  pObj: device object dm916x_Object_t.
  * @retval DM916x_STATUS_OK  if OK
  *         DM916x_STATUS_ADDRESS_ERROR if cannot find device address
  *         DM916x_STATUS_READ_ERROR if cannot read register
  *         DM916x_STATUS_WRITE_ERROR if cannot write to register
  *         DM916x_STATUS_RESET_TIMEOUT if cannot perform a software reset
  */
int32_t DM916x_Init(dm916x_Object_t *pObj)
{
	int32_t status = DM916x_STATUS_ADDRESS_ERROR;
	uint32_t tickstart = 0, regvalue = 0, addr = 0;
	
	if(pObj->Is_Initialized == 0) {
		if(pObj->IO.Init != 0) {
			pObj->IO.Init();
		}
		// enum the dev addr
		pObj->DevAddr = DM916x_MAX_DEV_ADDR + 1;
		for(addr = 0; addr <= DM916x_MAX_DEV_ADDR; addr++) {
			if(pObj->IO.ReadReg(addr, DM916x_REG_PHYIDR1, &regvalue) < 0) {
				PHY_ERR("read PHY addr[0x%02x] err\n", addr);
				status = DM916x_STATUS_READ_ERROR;
				continue;
			}
			if(regvalue == PHY_ID1) {
				PHY_DBG("verify PHY addr[0x%02x] OK\n", addr);
				pObj->DevAddr = addr;
				status = DM916x_STATUS_OK;
				break;
			} else {
				//PHY_ERR("verify PHY addr[0x%02x] err[0x%04x]\n", addr, regvalue);
			}
		}
		
		if(status != DM916x_STATUS_OK) {
			PHY_ERR("NOT found PHY chip\n");
		} else {	// reset device
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMCR, &regvalue) < 0) {
				PHY_ERR("read reg[0x%02x] err\n", DM916x_REG_BMCR);
				status = DM916x_STATUS_READ_ERROR;
			} else {
				if(pObj->IO.WriteReg(pObj->DevAddr, DM916x_REG_BMCR, regvalue | BMCR_RESET) < 0) {
					PHY_ERR("write reg[0x%02x] err\n", DM916x_REG_BMCR);
					status = DM916x_STATUS_WRITE_ERROR;
				} else {
					//PHY_DBG("write 0x%04x to reg[0x%02x] OK\n", regvalue | BMCR_RESET, DM916x_REG_BMCR);
					if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMCR, &regvalue) >= 0) {
						tickstart = pObj->IO.GetTick();
						/* wait until software reset is done or timeout occurred */
						while(regvalue & BMCR_RESET) {
							if((pObj->IO.GetTick() - tickstart) <= DM916x_SW_RESET_TO) {
								if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMCR, &regvalue) < 0) {
									PHY_ERR("read reg[0x%02x] err\n", DM916x_REG_BMCR);
									status = DM916x_STATUS_READ_ERROR;
									break;
								}
							} else {
								PHY_ERR("DM916x reset timeout\n");
								status = DM916x_STATUS_RESET_TIMEOUT;
								break;
							}
						}
					} else {
						PHY_ERR("read reg[0x%02x] err\n", DM916x_REG_BMCR);
						status = DM916x_STATUS_READ_ERROR;
					}
				}
			}
    }
		// wait for dev init finish
		if(status == DM916x_STATUS_OK) {
			PHY_DBG("DM916x reset OK\n");
			tickstart =  pObj->IO.GetTick();
			/* Wait for 2s to perform initialization */
			while((pObj->IO.GetTick() - tickstart) <= DM916x_INIT_TO) {}
			pObj->Is_Initialized = 1;
				
			#if 0
			// read DM916x registers
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMCR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_BMCR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_BMCR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMSR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_BMSR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_BMSR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_PHYIDR1, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_PHYIDR1);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_PHYIDR1, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_PHYIDR2, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_PHYIDR2);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_PHYIDR2, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_ANAR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_ANAR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_ANAR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_ANLPAR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_ANLPAR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_ANLPAR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_ANER, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_ANER);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_ANER, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_DSCR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_DSCR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_DSCR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_DSCSR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_DSCSR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_DSCSR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_10BTCSR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_10BTCSR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_10BTCSR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_PWDOR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_PWDOR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_PWDOR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_CONGFIG, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_CONGFIG);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_CONGFIG, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_INTERRUPT, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_INTERRUPT);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_INTERRUPT, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_SRECR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_SRECR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_SRECR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_DISCR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_DISCR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_DISCR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_RLSR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_RLSR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_RLSR, regvalue);
			}
			if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_PSCR, &regvalue) < 0) {
				PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_PSCR);
			} else {
				PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_PSCR, regvalue);
			}
			#endif
		}
	} else {
		status = DM916x_STATUS_OK;
	}
	
	return status;
}

/**
  * @brief  Get the link state of LAN8742 device.
  * @param  pObj: Pointer to device object.
  * @param  pLinkState: Pointer to link state
  * @retval DM916x_STATUS_LINK_DOWN  if link is down
  *         DM916x_STATUS_AUTONEGO_NOTDONE if Auto nego not completed
  *         DM916x_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         DM916x_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         DM916x_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         DM916x_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD
  *         DM916x_STATUS_READ_ERROR if cannot read register
  *         DM916x_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t DM916x_GetLinkState(dm916x_Object_t *pObj)
{
	uint32_t val = 0;
	static uint8_t flag = 0;
	if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMSR, &val) < 0) {
		PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_BMSR);
		return  DM916x_STATUS_READ_ERROR;
	} else {
		//PHY_DBG("MD916x read reg[0x%02x] 0x%04x\n", DM916x_REG_BMSR, val);
	}
	
	if((val & BMSR_LINK_STAT) != BMSR_LINK_STAT) {
		if(is_dm916x_link_up != DM916x_LINK_STAT_DOWN && is_dm916x_link_up != 0) {
			PHY_DBG("DM916x Link Down\n");
			is_dm916x_link_up = DM916x_LINK_STAT_DOWN;
		}
		extern volatile uint8_t is_udp_connect;
		extern void udpClient_disconnect(void);
		is_udp_connect = 0;
		udpClient_disconnect();
		return DM916x_STATUS_LINK_DOWN;
	} else {
		if(is_dm916x_link_up != DM916x_LINK_STAT_UP) {
			is_dm916x_link_up = DM916x_LINK_STAT_UP;
			flag = 1;
		}
	}
	
	if((val & BMSR_ANEG_COMPLETE) != BMSR_ANEG_COMPLETE) {
		//PHY_ERR("DM916x AutoNego not completed\n");
    return DM916x_STATUS_AUTONEGO_NOTDONE;
	}
	
	if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_BMCR, &val) < 0) {
		PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_BMCR);
		return DM916x_STATUS_READ_ERROR;
	} else {
		//PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_BMCR, val);
	}
	
	int32_t ret;
	if((val & BMCR_ANEG_EN) == 0) {		// Auto Nego disableed
		if(((val & BMCR_SPEED_SELECT) == BMCR_SPEED_SELECT) && ((val & BMCR_DUPLEX_MODE) == BMCR_DUPLEX_MODE)) {
			ret = DM916x_STATUS_100MBITS_FULLDUPLEX;
		} else if((val & BMCR_SPEED_SELECT) == BMCR_SPEED_SELECT) {
			ret = DM916x_STATUS_100MBITS_HALFDUPLEX;
		} else if((val & BMCR_DUPLEX_MODE) == BMCR_DUPLEX_MODE) {
			ret = DM916x_STATUS_10MBITS_FULLDUPLEX;
		} else {
			ret = DM916x_STATUS_10MBITS_HALFDUPLEX;
		}
		if(flag) {
			flag = 0;
			PHY_DBG("\nDM916x Link UP\n");
			PHY_DBG("Auto-Nego OFF, %s %s mode\n", (ret & 0x2)?"100Mbps":"10Mbps", (ret & 0x1)?"Full-Duplex":"Half-Duplex");
		}
		return ret;
	} else {		// Auto Nego enabled		
		if(pObj->IO.ReadReg(pObj->DevAddr, DM916x_REG_DSCSR, &val) < 0) {
			PHY_ERR("DM916x read reg[0x%02x] err\n", DM916x_REG_DSCSR);
      return DM916x_STATUS_READ_ERROR;
    } else {
			//PHY_DBG("DM916x read reg[0x%02x] 0x%04x\n", DM916x_REG_DSCSR, val);
		}
		if((val & 0xF) != 0x8) {
      return DM916x_STATUS_AUTONEGO_NOTDONE;
    }
		
		if((val & DSCSR_100M_FD) == DSCSR_100M_FD) {
			ret = DM916x_STATUS_100MBITS_FULLDUPLEX;
		} else if((val & DSCSR_100M_HD) == DSCSR_100M_HD) {
			ret = DM916x_STATUS_100MBITS_HALFDUPLEX;
		} else if((val & DSCSR_10M_FD) == DSCSR_10M_FD) {
			ret = DM916x_STATUS_10MBITS_FULLDUPLEX;
		} else {
			ret = DM916x_STATUS_10MBITS_HALFDUPLEX;
		}
		if(flag) {
			flag = 0;
			PHY_DBG("\nDM916x Link UP\n");
			PHY_DBG("Auto-Nego ON, %s %s mode\n", (ret & 0x2)?"100Mbps":"10Mbps", (ret & 0x1)?"Full-Duplex":"Half-Duplex");
		}
		return ret;
	}
}

/**
  * @brief  Register IO functions to component object
  * @param  pObj: device object of dm916x_Object_t.
  * @param  ioctx: holds device IO functions.
  * @retval DM916x_STATUS_OK  if OK
  *         DM916x_STATUS_ERROR if missing mandatory function
  */
int32_t  DM916x_RegisterBusIO(dm916x_Object_t *pObj, dm916x_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return DM916x_STATUS_ERROR;
  }

  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;

  return DM916x_STATUS_OK;
}
