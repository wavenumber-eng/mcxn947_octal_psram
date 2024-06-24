#include "bunny_mem.h"
#include <stdbool.h>
#include <stdint.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_flexspi.h"
#include "fsl_cache.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_edma.h"
#include <stdio.h>


#define BUNNY_FLEXSPI 				  	   FLEXSPI0
#define BUNNY_FLEXSPI_BASE_ADDRESS 		   FlexSPI0_AMBA_BASE
#define BUNNY_FLEXSPI_PORT 			  	   kFLEXSPI_PortA1

#define HYPERRAM_CMD_LUT_SEQ_IDX_READDATA  0
#define HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA 1
#define HYPERRAM_CMD_LUT_SEQ_IDX_READREG   2
#define HYPERRAM_CMD_LUT_SEQ_IDX_WRITEREG  3
#define HYPERRAM_CMD_LUT_SEQ_IDX_RESET     4

volatile uint32_t CycleTimer = 0;
volatile uint32_t CycleOffset = 0;
volatile uint32_t cycle_cnt = 0;

#define INIT_CYCLE_TIMER			        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;SysTick->LOAD = 0xFFFFFF;SysTick->VAL = 0;
#define START_CYCLE_TIMER					SysTick->VAL = 0;SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
#define STOP_AND_GRAB_CYCLE_TIMER(x)	    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;	x++; x = SysTick->VAL; x = 0x1000000 - x;
#define REPORT_CYCLE_TIMER(x)			   	(x - CycleOffset)
#define CALIBRATE_CYCLE_TIMER		   		INIT_CYCLE_TIMER;START_CYCLE_TIMER;STOP_AND_GRAB_CYCLE_TIMER(CycleOffset)

uint8_t write_test_block[CONFIG__MAX_BLOCK_SIZE]		 	__attribute__(( aligned(32))) ;
uint8_t read_test_block[CONFIG__MAX_BLOCK_SIZE]		 		__attribute__(( aligned(32))) ;
uint8_t psram_spoof[CONFIG__MAX_BLOCK_SIZE]		 			__attribute__(( aligned(32))) ;

uint32_t random_write_address_buf_1[CONFIG__RANDOM_BLOCK_SIZE];
uint32_t random_read_address_buf_1[CONFIG__RANDOM_BLOCK_SIZE];

edma_transfer_config_t transferConfig;
edma_config_t userConfig;
volatile bool g_Transfer_Done   = false;

flexspi_device_config_t psram_config =
{
    .flexspiRootClk = 133333,
    .isSck2Enabled = false,
    .flashSize = 0x2000,
    .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval = 2,
    .CSHoldTime = 2,
    .CSSetupTime = 2,
    .dataValidTime = 2, //Important for APS6408L-3OBM-BA
    .enableWordAddress = false,
    .AWRSeqIndex = 1,
    .AWRSeqNumber = 1,
    .ARDSeqIndex = 0,
    .ARDSeqNumber = 1,
    .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval = 0,
    .enableWriteMask = true,
};

uint32_t psram_octal_lut[64] =
 {
    /* Read Data */
    [0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20),

    [1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07, kFLEXSPI_Command_READ_DDR,
                          kFLEXSPI_8PAD, 0x04),

    /* Write Data */
    [4] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20),
    [5] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07, kFLEXSPI_Command_WRITE_DDR,
                          kFLEXSPI_8PAD, 0x04),

    /* Read Register */
    [8] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x40, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20),
    [9] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x07, kFLEXSPI_Command_READ_DDR,
                          kFLEXSPI_8PAD, 0x04),

    /* Write Register */
    [12] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0xC0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20),
    [13] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x08, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD,
                           0x00),

    /* Reset */
    [16] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0xFF, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, 0x03),

};

void EDMA_1_CH0_IRQHandler(void)
{
    if ((EDMA_GetChannelStatusFlags(DMA1, 0U) & kEDMA_InterruptFlag) != 0U)
    {
        EDMA_ClearChannelStatusFlags(DMA1, 0U, kEDMA_InterruptFlag);
        g_Transfer_Done = true;
    }
}

status_t flexspi_hyper_ram_ipcommand_write_data(FLEXSPI_Type *base,
												uint32_t address,
												uint32_t *buffer,
												uint32_t length)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
    flashXfer.deviceAddress = address;
    flashXfer.port = BUNNY_FLEXSPI_PORT;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA;
    flashXfer.data = buffer;
    flashXfer.dataSize = length;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_hyper_ram_ipcommand_read_data(FLEXSPI_Type *base,
											   uint32_t address,
											   uint32_t *buffer,
											   uint32_t length)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Read data */
    flashXfer.deviceAddress = address;
    flashXfer.port = BUNNY_FLEXSPI_PORT;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = HYPERRAM_CMD_LUT_SEQ_IDX_READDATA;
    flashXfer.data = buffer;
    flashXfer.dataSize = length;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t flexspi_hyper_ram_write_mcr(FLEXSPI_Type *base,
											uint8_t regAddr,
											uint32_t *mrVal)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
    flashXfer.deviceAddress = regAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = 3;
    flashXfer.data = mrVal;
    flashXfer.dataSize = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t flexspi_hyper_ram_get_mcr(FLEXSPI_Type *base,
										  uint8_t regAddr,
										  uint32_t *mrVal)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Read data */
    flashXfer.deviceAddress = regAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = 2;
    flashXfer.data = mrVal;
    flashXfer.dataSize = 2;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t flexspi_hyper_ram_reset(FLEXSPI_Type *base)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
    flashXfer.deviceAddress = 0x0U;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = 4;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status == kStatus_Success)
    {
        for (uint32_t i = 2000000U; i > 0; i--)
        {
            __NOP();
        }
    }
    return status;
}

uint32_t bunny_mem__write_continuous_block(uint32_t psram_offset,
										   uint8_t* in_block,
										   uint32_t block_size,
										   bool use_dma);

uint32_t bunny_mem__read_continuous_block(uint32_t psram_offset,
										  uint8_t* in_block,
										  uint32_t block_size,
										  bool use_dma);

status_t bunny_mem__init(bool enable_cache)
{

	/*
	 * DMA 1 used for memory tests
	 *
	 */

	edma_config_t userConfig;
    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(DMA1, &userConfig);
    EnableIRQ(EDMA_1_CH0_IRQn);

    uint32_t mr0mr1[1];
    uint32_t mr4mr8[1];
    uint32_t mr0Val[1];
    uint32_t mr4Val[1];
    uint32_t mr8Val[1];
    flexspi_config_t config;
    cache64_config_t cacheCfg;
    status_t status = kStatus_Success;

    RESET_PeripheralReset(kFLEXSPI_RST_SHIFT_RSTn);

    if(enable_cache)
    {
		/* As cache depends on FlexSPI power and clock, cache must be initialized after FlexSPI power/clock is set */
		CACHE64_GetDefaultConfig(&cacheCfg);
		CACHE64_Init(CACHE64_POLSEL0, &cacheCfg);

		CACHE64_EnableWriteBuffer(CACHE64_CTRL0, true);
		CACHE64_EnableCache(CACHE64_CTRL0);
    }

    /* Get FLEXSPI default settings and configure the flexspi. */
    FLEXSPI_GetDefaultConfig(&config);


    config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;

    /*Set AHB buffer size for reading data through AHB bus. */
    config.ahbConfig.enableAHBPrefetch = true;
    config.ahbConfig.enableAHBBufferable = true;
    config.ahbConfig.enableAHBCachable = true;
    config.ahbConfig.enableReadAddressOpt = true;

    for (uint8_t i = 1; i < FSL_FEATURE_FLEXSPI_AHB_BUFFER_COUNT - 1; i++)
    {
        config.ahbConfig.buffer[i].bufferSize = 0;
    }

    config.ahbConfig.buffer[0].masterIndex = 4;  /* DMA0 */
    config.ahbConfig.buffer[0].bufferSize = 512; /* Allocate 512B bytes for DMA0 */
    config.ahbConfig.buffer[0].enablePrefetch = true;
    config.ahbConfig.buffer[0].priority = 0;
    /* All other masters use last buffer with 512B bytes. */
    config.ahbConfig.buffer[FSL_FEATURE_FLEXSPI_AHB_BUFFER_COUNT - 1].bufferSize = 512;
#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN)
    config.enableCombination = true;
#endif

    FLEXSPI_Init(BUNNY_FLEXSPI, &config);

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig(BUNNY_FLEXSPI, &psram_config, kFLEXSPI_PortA1);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT(BUNNY_FLEXSPI, 0, psram_octal_lut, ARRAY_SIZE(psram_octal_lut));

    /* Do software reset. */
    FLEXSPI_SoftwareReset(BUNNY_FLEXSPI);

    /* Reset hyper ram. */
    status = flexspi_hyper_ram_reset(BUNNY_FLEXSPI);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_hyper_ram_get_mcr(BUNNY_FLEXSPI, 0x0, mr0mr1);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = flexspi_hyper_ram_get_mcr(BUNNY_FLEXSPI, 0x4, mr4mr8);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable RBX, burst length set to 1K. - MR8 */
    mr8Val[0] = (mr4mr8[0] & 0xFF00U) >> 8U;

    mr8Val[0] = mr8Val[0] | 0x0F;

    status = flexspi_hyper_ram_write_mcr(BUNNY_FLEXSPI, 0x8, mr8Val);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Set LC code to 0x04(LC=7, maximum frequency 200M) - MR0. */
    mr0Val[0] = mr0mr1[0] & 0x00FFU;
    mr0Val[0] = (mr0Val[0] & ~0x3CU) | (4U << 2U);

    status = flexspi_hyper_ram_write_mcr(BUNNY_FLEXSPI, 0x0, mr0Val);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Set WLC code to 0x01(WLC=7, maximum frequency 200M) - MR4. */
    mr4Val[0] = mr4mr8[0] & 0x00FFU;
    mr4Val[0] = (mr4Val[0] & ~0xE0U) | (1U << 5U);
    status = flexspi_hyper_ram_write_mcr(BUNNY_FLEXSPI, 0x4, mr4Val);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Need to reset FlexSPI controller between IP/AHB access. */
    FLEXSPI_SoftwareReset(BUNNY_FLEXSPI);

    return status;
}

bool bunny_mem__test(uint32_t block_size, uint32_t iterations)
{
    status_t status;

    CALIBRATE_CYCLE_TIMER;

    status = bunny_mem__init(true);

    if (status != kStatus_Success)
    {
        assert(false);
    }

    memset(read_test_block, 0, sizeof(read_test_block));

	BUNNY_DBG(VT100_HIDE_CURSOR);

    for (int x = 0; x < iterations; x++)
    {
        for (int i = 0; i < CONFIG__BUNNY_DRAM_SIZE; i += block_size)
        {

           BUNNY_DBG(VT100_WHITE "Testing block at address:" VT100_YELLOW          \
            		  "0x%x" VT100_GREEN "  Iteration: " VT100_MAGENTA "%i / %i\r", \
					  i + FlexSPI0_AMBA_BASE, x+1, iterations);

            for (int j = 0; j < sizeof(block_size); j++)
            {
            	write_test_block[j] = rand();
            }

            bunny_mem__write_continuous_block(i,
            								  (uint8_t *)write_test_block,
                                              block_size,
										   	  true);


            bunny_mem__read_continuous_block(i,
            								 (uint8_t *)read_test_block,
            								 block_size,
											 true);

            int k = memcmp(write_test_block,read_test_block,block_size);

            if(k)
            {
            	  BUNNY_DBG("Failure at index %d  R0x%02x :  W0x%02x", k, read_test_block[k], write_test_block[k]);

            	  BUNNY_DBG("%02x\r\n", ((read_test_block[k] ^ write_test_block[k]) & 0xFF));

            	  return false;
            }
        }
    }

    CLEAR_LINE();

    BUNNY_DBG(VT100_WHITE"\r\n\r\nPSRAM test complete.\r\n\r\n");

    BUNNY_DBG(VT100_GREEN"    ""********************************\r\n");
    BUNNY_DBG(VT100_GREEN"    ""*         !Victory!            *\r\n");
    BUNNY_DBG(VT100_GREEN"    ""*     Passed @ all addresses   *\r\n");
    BUNNY_DBG(VT100_GREEN"    ""*        %d bytes OK      *\r\n", CONFIG__BUNNY_DRAM_SIZE);
    BUNNY_DBG(VT100_GREEN"    ""*  You win a "VT100_CYAN"blueberry muffin"VT100_GREEN"  *\r\n");
    BUNNY_DBG(VT100_GREEN"    ""********************************\r\n");

    BUNNY_DBG("\r\n");

    BUNNY_DBG(VT100_WHITE);

    return true;
}

// Fill the in_block with pseudo random numbers
void bunny_mem__fill_random_block(uint8_t* in_block, uint32_t block_size){

    for (int j = 0; j < block_size; j++)
    {
        in_block[j] = rand();
    }
}


uint32_t bunny_mem__write_continuous_block(uint32_t psram_offset,
										   uint8_t* in_block,
										   uint32_t block_size,
										   bool use_dma)
{
	#if CONFIG__PSRAM_SPOOF == 0
		volatile uint32_t *startAddr = (volatile uint32_t *)(BUNNY_FLEXSPI_BASE_ADDRESS + psram_offset);
	#else
		volatile uint32_t *startAddr = (volatile uint32_t *)&psram_spoof[0];
	#endif


	if(use_dma)
	{
		uint32_t transfer_bytes = sizeof(write_test_block[0]) * block_size;

		EDMA_PrepareTransfer(&transferConfig,
							    in_block,
								32,
								(void *)startAddr,
								32,
								transfer_bytes,
								transfer_bytes,
								kEDMA_MemoryToMemory);

		g_Transfer_Done = 0;

		EDMA_SetTransferConfig(DMA1, 0U, &transferConfig, NULL);

		INIT_CYCLE_TIMER;
		START_CYCLE_TIMER;
		EDMA_TriggerChannelStart(DMA1, 0U);

		while (g_Transfer_Done != true);

		STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);
	}
	else
	{
		INIT_CYCLE_TIMER;
		START_CYCLE_TIMER;

		memcpy((void *)startAddr,(void *)in_block, block_size);

		STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);
	}

	return REPORT_CYCLE_TIMER(cycle_cnt);

}

uint32_t bunny_mem__read_continuous_block(uint32_t psram_offset,
										  uint8_t* in_block,
										  uint32_t block_size,
										  bool use_dma)
{
	#if CONFIG__PSRAM_SPOOF == 0
		volatile uint32_t *startAddr = (volatile uint32_t *)(BUNNY_FLEXSPI_BASE_ADDRESS + psram_offset);
	#else
		volatile uint32_t *startAddr = (volatile uint32_t *)&psram_spoof[0];
	#endif


	if(use_dma)
   	{
   		uint32_t transfer_bytes = sizeof(write_test_block[0]) * block_size;


   		EDMA_PrepareTransfer(&transferConfig,
   								(void *)startAddr,
   								32,
								in_block,
   								32,
   								transfer_bytes,
   								transfer_bytes,
   								kEDMA_MemoryToMemory);
   		g_Transfer_Done = 0;


   		EDMA_SetTransferConfig(DMA1, 0U, &transferConfig, NULL);
		INIT_CYCLE_TIMER;
		START_CYCLE_TIMER;
   		EDMA_TriggerChannelStart(DMA1, 0U);
   		while (g_Transfer_Done != true);
   		STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);
   	}
   	else
   	{
   		START_CYCLE_TIMER;
   		memcpy((void *)in_block, (void *)startAddr, block_size);
   		STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);
   	}

	return REPORT_CYCLE_TIMER(cycle_cnt);

}

uint32_t bunny_mem__write_to_random_address(uint32_t *write_block,
								   uint32_t *write_addr_buf,
								   uint32_t test_block_size)
{
	#if CONFIG__PSRAM_SPOOF == 0
		volatile uint32_t *psram = (volatile uint32_t *)(BUNNY_FLEXSPI_BASE_ADDRESS);
	#else
		volatile uint32_t *psram = (volatile uint32_t *)&psram_spoof[0];
	#endif

	INIT_CYCLE_TIMER;
	START_CYCLE_TIMER;

    for (int i = 0; i < test_block_size; i++)
    {
    	psram[write_addr_buf[i]] = write_block[i];
    }

	STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);

	return REPORT_CYCLE_TIMER(cycle_cnt);
}

uint32_t bunny_mem__read_from_random_address(uint32_t *read_block,
								   uint32_t *read_addr_buff,
								   uint32_t test_block_size)
	{
	#if CONFIG__PSRAM_SPOOF == 0
		volatile uint32_t *psram = (volatile uint32_t *)(BUNNY_FLEXSPI_BASE_ADDRESS);
	#else
		volatile uint32_t *psram = (volatile uint32_t *)&psram_spoof[0];
	#endif

	INIT_CYCLE_TIMER;
	START_CYCLE_TIMER;

    for (int i = 0; i < test_block_size; i++)
    {
    	read_block[i] = psram[read_addr_buff[i]];
    }

	STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);

	return REPORT_CYCLE_TIMER(cycle_cnt);
}


uint32_t bunny_mem__write__then_read_to_random_address(uint32_t *write_block,
								   uint32_t *write_addr_buf,
								   uint32_t *read_block,
								   uint32_t *read_addr_buff,
								   uint32_t test_block_size)
{

	#if CONFIG__PSRAM_SPOOF == 0
		volatile uint32_t *psram = (volatile uint32_t *)(BUNNY_FLEXSPI_BASE_ADDRESS);
	#else
		volatile uint32_t *psram = (volatile uint32_t *)&psram_spoof[0];
	#endif


	INIT_CYCLE_TIMER;
	START_CYCLE_TIMER;

    for (int i = 0; i < test_block_size; i++)
    {
    	psram[write_addr_buf[i]] = write_block[i];
    	read_block[i] = psram[read_addr_buff[i]];
    }

	STOP_AND_GRAB_CYCLE_TIMER(cycle_cnt);

	return REPORT_CYCLE_TIMER(cycle_cnt);
}


void bunny_mem__speed_test__blocks(bool enable_cache,
								  uint32_t test_block_size,
								  uint32_t iterations,
								  bunny_mem_speed_test_blocks__result__t *bmtr)
{
    status_t status;

    if(iterations == 0) { iterations = 1; }

    psram_config.dataValidTime = 2;

    status = bunny_mem__init(enable_cache);

    if (status != kStatus_Success){
        assert(false);
    }

    bunny_mem__fill_random_block(write_test_block, test_block_size);

    uint64_t acc;

    acc = 0;
    for(int i=0 ; i< iterations ; i++)
    {
    	acc +=  bunny_mem__write_continuous_block(0,write_test_block, test_block_size,true);
    }
    bmtr->cycle_count__dma_write  = acc / iterations;

    acc = 0;
    for(int i=0 ; i< iterations ; i++)
    {
    	acc +=  bunny_mem__read_continuous_block(0,read_test_block, test_block_size,true);
    }
    bmtr->cycle_count__dma_read  = acc / iterations;

    acc = 0;
    for(int i=0 ; i< iterations ; i++)
    {
    	acc +=  bunny_mem__write_continuous_block(0,write_test_block, test_block_size,false);
    }
    bmtr->cycle_count__memcpy_write  = acc / iterations;

    acc = 0;
    for(int i=0 ; i< iterations ; i++)
    {
    	acc +=  bunny_mem__read_continuous_block(0,read_test_block, test_block_size,false);
    }
    bmtr->cycle_count__memcpy_read  = acc / iterations;

    if(memcmp(write_test_block,read_test_block,test_block_size) != 0)
    {
    	BUNNY_DBG("Read/Write Mismatch\r\n");
    }
}


void  bunny_mem__speed_test__random_addr(bool enable_cache,
										 uint32_t test_block_size,
										 uint32_t iterations,
										 bunny_mem_speed_test_random_access__result__t  *btr)
{

    uint64_t acc[3] = {0,0,0};
    uint32_t spread = 16384;

    for(int j=0 ; j< iterations ; j++)
    {


    	for(int i=0; i<test_block_size/4; i++)
		{
			random_read_address_buf_1[i] = (rand()%spread)/4;  //doing 32-bit transactions
			random_write_address_buf_1[i] = (rand()%spread)/4; //doing 32-bit transactions
		}


		bunny_mem__fill_random_block(write_test_block, test_block_size);


		acc[0] += bunny_mem__write_to_random_address((uint32_t *)write_test_block,
												  (uint32_t *)random_write_address_buf_1,
												  test_block_size/4 //doing 32-bit transactions
												  );
		acc[1] += bunny_mem__read_from_random_address((uint32_t *)read_test_block,
												  (uint32_t *)random_read_address_buf_1,
												  test_block_size/4  //doing 32-bit transactions
												  );
		acc[2] += bunny_mem__write__then_read_to_random_address((uint32_t *)write_test_block,
												  (uint32_t *)random_write_address_buf_1,
												  (uint32_t *)read_test_block,
												  (uint32_t *)random_read_address_buf_1,
												  test_block_size/4 //doing 32-bit transactions
												  );
    }

    btr->cycle_count__cache_range_write_only = acc[0]/iterations;
    btr->cycle_count__cache_range_read_only = acc[1]/iterations;
    btr->cycle_count__cache_range_write_then_read = acc[2]/iterations;


    acc[0] = 0;
    acc[1] = 0;
    acc[2] = 0;

    spread = CONFIG__BUNNY_DRAM_SIZE;

    for(int j=0 ; j< iterations ; j++)
    {

    	for(int i=0; i<test_block_size/4; i++)
		{
			random_read_address_buf_1[i] = (rand()%spread)/4;  //doing 32-bit transactions
			random_write_address_buf_1[i] = (rand()%spread)/4; //doing 32-bit transactions
		}

		bunny_mem__fill_random_block(write_test_block, test_block_size);


		acc[0] += bunny_mem__write_to_random_address((uint32_t *)write_test_block,
												  (uint32_t *)random_write_address_buf_1,
												  test_block_size/4 //doing 32-bit transactions
												  );
		acc[1] += bunny_mem__read_from_random_address((uint32_t *)read_test_block,
												  (uint32_t *)random_read_address_buf_1,
												  test_block_size/4  //doing 32-bit transactions
												  );
		acc[2] += bunny_mem__write__then_read_to_random_address((uint32_t *)write_test_block,
												  (uint32_t *)random_write_address_buf_1,
												  (uint32_t *)read_test_block,
												  (uint32_t *)random_read_address_buf_1,
												  test_block_size/4 //doing 32-bit transactions
												  );
    }

    btr->cycle_count__fullspan_write_only = acc[0]/iterations;
    btr->cycle_count__fullspan_read_only = acc[1]/iterations;
    btr->cycle_count__fullspan_write_then_read = acc[2]/iterations;

}

