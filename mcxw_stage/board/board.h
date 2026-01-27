
#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include <stdbool.h>

//LPSPI_Type
#include "fsl_common.h"

//spi_pin_mode_t
#include "fsl_lpspi_mem_adapter.h"

//needed by fsl_lpspi_mem_adapter.c
void BOARD_LpspiPcsPinControl(bool isSelected);
void BOARD_LpspiIomuxConfig(spi_pin_mode_t pinMode);

//needed by fsl_lpspi_nor_flash.c
uint32_t BOARD_GetLpspiClock(void);
uint32_t BOARD_GetNorFlashBaudrate(void);
LPSPI_Type *BOARD_GetLpspiForNorFlash(void);

#endif //_BOARD_H_
