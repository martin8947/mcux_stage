
#include <assert.h>
#include <stdint.h>

#include "iic_test.h"

#include "board.h"
#include "clock_config.h"
#include "peripherals.h"
#include "pin_mux.h"

volatile uint8_t iic_test_flag_done;

void iic_test_xfer_cb(LPI2C_Type *base, lpi2c_master_edma_handle_t *handle, status_t completionStatus, void *userData)
{
	//assert(completionStatus == kStatus_Success);
	iic_test_flag_done = 1;
}

void iic_test(void)
{
	lpi2c_master_transfer_t xfer;
    uint8_t data[16];

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = 0x19;
    xfer.subaddressSize = 1;
    xfer.data = &data;

    //test read
    xfer.direction = kLPI2C_Read;
    xfer.subaddress = 0x12;
    xfer.dataSize = 16;
    iic_test_flag_done = 0;
    assert(LPI2C_MasterTransferEDMA(LPI2C1_PERIPHERAL, &LPI2C1_masterHandle, &xfer) == kStatus_Success);
    while (!iic_test_flag_done);
    while (1);
}
