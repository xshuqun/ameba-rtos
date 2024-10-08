/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"


/**
  *  @brief Config all pins to the right function and pull state based on pmap_func table.
  *  @note should set pmap_func. Function & FuncPuPd corectly based on your board.
  *  @retval None
  */

void pinmap_init(void)
{
	int i = 0;
	u32 PuCtrl[2], PdCtrl[2];
	u32 SlpPuCtrl[2], SlpPdCtrl[2];
	u8 PortNum, PinNum;

	/* share audio pin */
	//HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_AUD_PAD_CTRL, 0x1FFFFF);

	PuCtrl[0] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_1);
	PdCtrl[0] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_1);
	PuCtrl[1] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_2);
	PdCtrl[1]  = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_2);

	SlpPuCtrl[0] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_SLP_1);
	SlpPdCtrl[0] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_SLP_1);
	SlpPuCtrl[1] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_SLP_2);
	SlpPdCtrl[1] = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_SLP_2);

	for (;;) {
		/*  Check if search to end */
		if (pmap_func[i].PinName == _PNC) {
			break;
		}

		/* HW PMC enable all the PAD by default, shutdown if do not need.*/
		if (pmap_func[i].FuncPuPd == GPIO_PuPd_SHUTDOWN) {
			PAD_CMD(pmap_func[i].PinName, DISABLE);
		}
#if 0
		/* keep active level when GPIO_PuPd_KEEP */
		if (pmap_func[i].FuncPuPd != GPIO_PuPd_KEEP) {
			PAD_PullCtrl(pmap_func[i].PinName, pmap_func[i].FuncPuPd);
		}

		/* keep sleep setting level when GPIO_PuPd_KEEP */
		if (pmap_func[i].SleepPuPd != GPIO_PuPd_KEEP) {
			PAD_SleepPullCtrl(pmap_func[i].PinName, pmap_func[i].SleepPuPd);
		}
#endif

		/*The pad control are placed in always on power domain,
			each acess may cost hundreds microsconds*/
		PortNum = PORT_NUM(pmap_func[i].PinName);
		PinNum = PIN_NUM(pmap_func[i].PinName);

		if (pmap_func[i].FuncPuPd == GPIO_PuPd_DOWN) {
			PuCtrl[PortNum] &= ~(BIT(PinNum));
			PdCtrl[PortNum] |= BIT(PinNum);
		} else if (pmap_func[i].FuncPuPd == GPIO_PuPd_UP) {
			PuCtrl[PortNum] |= BIT(PinNum);
			PdCtrl[PortNum] &= ~(BIT(PinNum));
		} else if (pmap_func[i].FuncPuPd == GPIO_PuPd_NOPULL) {
			PuCtrl[PortNum] &= ~(BIT(PinNum));
			PdCtrl[PortNum] &= ~(BIT(PinNum));
		}

		if (pmap_func[i].SleepPuPd == GPIO_PuPd_DOWN) {
			SlpPuCtrl[PortNum] &= ~(BIT(PinNum));
			SlpPdCtrl[PortNum] |= BIT(PinNum);
		} else if (pmap_func[i].SleepPuPd == GPIO_PuPd_UP) {
			SlpPuCtrl[PortNum] |= BIT(PinNum);
			SlpPdCtrl[PortNum] &= ~(BIT(PinNum));
		} else if (pmap_func[i].SleepPuPd == GPIO_PuPd_NOPULL) {
			SlpPuCtrl[PortNum] &= ~(BIT(PinNum));
			SlpPdCtrl[PortNum] &= ~(BIT(PinNum));
		}

		i++;
	}


	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PU_1, PuCtrl[0]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PD_1, PdCtrl[0]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PU_SLP_1, SlpPuCtrl[0]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PD_SLP_1, SlpPdCtrl[0]);

	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PU_2, PuCtrl[1]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PD_2, PdCtrl[1]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PU_SLP_2, SlpPuCtrl[1]);
	HAL_WRITE32(PINMUX_REG_BASE, REG_PAD_PD_SLP_2, SlpPdCtrl[1]);
}