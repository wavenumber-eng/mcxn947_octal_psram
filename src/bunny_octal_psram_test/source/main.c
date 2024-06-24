#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "bunny_mem.h"
#include "bunny_debug.h"
#include "fsl_edma.h"

#define NUM_MEM_TESTS					(6)
#define NUM_COLUMNS						(sizeof(columns)/sizeof(char *))
#define CYCLES_TO_RATE(cycles,size)		((float)(size)*(SystemCoreClock/1000000.0f)/(float)cycles)

bunny_mem_speed_test_blocks__result__t mem_test_result[NUM_MEM_TESTS];

bunny_mem_speed_test_random_access__result__t btr;

uint32_t mem_test_size[NUM_MEM_TESTS];

#define START_COLUMN   				(0)
#define COLUMN_WIDTH   				(15)
#define MAX_COLUMN_SIZE	            (30) //Note: add some extra characters for color codes

const char *columns[] = {"block_size",
    			   "memcpy_read",
				   "dma_read",
				   "memcpy_write",
				   "dma_write"};

const char *column_colors[] = {VT100_WHITE,
						 VT100_WHITE,
						 VT100_WHITE,
						 VT100_WHITE,
						 VT100_WHITE,
    					};


char col_string[NUM_COLUMNS][MAX_COLUMN_SIZE];

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

   	BUNNY_DBG(VT100_CLEAR_SCREEN);
	BUNNY_DBG(VT100_HIDE_CURSOR);
   	BUNNY_DBG(VT100_CURSOR_HOME);
    BUNNY_DBG("\r\n");
    BUNNY_DBG(VT100_WHITE"    *----------------------------*\r\n");
    BUNNY_DBG(VT100_WHITE"    |	     / \\                 |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	    / _ \\                |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   | / \\ |               |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || _______       |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || |\\     \\      |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || ||\\     \\     |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || || \\    |     | \r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || ||  \\__/      |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   ||   || ||   ||       |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	    \\\\_/ \\_/ \\_//        |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	   /   _     _   \\       |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	  /               \\      |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	  |    "VT100_CYAN"O     "VT100_CYAN"O"VT100_WHITE"    |      |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	  |   \\  ___  /   |      |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	 /     \\ \\_/ /     \\     |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	/  -----  |  -----  \\    |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	|     \\__/|\\__/     |    |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	\\       |_|_|       /    |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	 \\_____       _____/     |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	       \\     /           |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	       |     |           |\r\n");
    BUNNY_DBG(VT100_WHITE"    |	       |     |           |\r\n");
    BUNNY_DBG(VT100_WHITE"    |----------------------------|\r\n");
    BUNNY_DBG(VT100_WHITE"    |        NXP MCX N94         |\r\n");
    BUNNY_DBG(VT100_WHITE"    |      OCTAL DDR PSRAM       |\r\n");
    BUNNY_DBG(VT100_WHITE"    |      PERFORMANCE TEST      |\r\n");
    BUNNY_DBG(VT100_WHITE"    *----------------------------*\r\n");
    BUNNY_DBG("\r\n");

	#if CONFIG__PSRAM_MEM_TEST == 1

    	//write/read 16kb chunks and perform two iteration through the array
		if(!bunny_mem__test(16384 , 2))
		{
			//Hang out on failure
			while(1){}
		}

	#endif

    CLEAR_LINE();

	bool cache_enabled = false;

	for(int test_run = 0;test_run <4 ; test_run++)
	{
		BUNNY_DBG("\r\n");
		BUNNY_DBG("*********************************************************************\r\n");
		BUNNY_DBG("*                           Test Run : %d                            *\r\n",(test_run+1));
		BUNNY_DBG("*********************************************************************\r\n");
		BUNNY_DBG("\r\n");

		switch(test_run)
		{
			default:
			case 0:
				cache_enabled = false;
				CLOCK_AttachClk(kPLL1_to_FLEXSPI);               //PLL1 is set to 133MHz
				BUNNY_DBG("FlexSPI clock set to  "VT100_GREEN" 133MHz "VT100_WHITE"\r\n");
				BUNNY_DBG("Running Block Memory test with "VT100_YELLOW" DISABLED CACHE64 [16KB] "VT100_WHITE"\r\n");
				break;
			case 1:
				cache_enabled = false;
				CLOCK_AttachClk(kPLL0_to_FLEXSPI);               //PLL0 is set to 150MHz
				BUNNY_DBG("FlexSPI clock set to  "VT100_YELLOW" 150MHz [!Overclock!]"VT100_WHITE"\r\n");
				BUNNY_DBG("Running Block Memory test with "VT100_YELLOW" DISABLED CACHE64 [16KB] "VT100_WHITE"\r\n");
				break;
			case 2:
				cache_enabled = true;
				CLOCK_AttachClk(kPLL1_to_FLEXSPI);               //PLL1 is set to 133MHz
				BUNNY_DBG("FlexSPI clock set to  "VT100_GREEN" 150MHz"VT100_WHITE"\r\n");
				BUNNY_DBG("Running Block Memory test with "VT100_GREEN" ENABLED CACHE64 [16KB] "VT100_WHITE"\r\n");
				break;
			case 3:
				cache_enabled = true;
				CLOCK_AttachClk(kPLL0_to_FLEXSPI);               //PLL0 is set to 150MHz

				BUNNY_DBG("FlexSPI clock set to  "VT100_YELLOW" 150MHz [!Overclock!]"VT100_WHITE"\r\n");
				BUNNY_DBG("Running Block Memory test with "VT100_GREEN" ENABLED CACHE64 [16KB] "VT100_WHITE"\r\n\r\n");
				break;
		}


		BUNNY_DBG("\r\n");

		BUNNY_DBG("Block access tests\r\n");
		BUNNY_DBG("-------------------------------------------------------------------------\r\n");

		BUNNY_DBG("\r\n");

		for(int i=0;i<NUM_COLUMNS;i++)
		{
			BUNNY_DBG("%s",column_colors[i]);
			PRINT_AT_COLUMN(START_COLUMN + (COLUMN_WIDTH   * i),columns[i]);
		}

		BUNNY_DBG("\r\n");

		MOVE_CURSOR_TO_COLUMN(START_COLUMN);

		BUNNY_DBG(VT100_WHITE);

		for(int i=0;i<NUM_COLUMNS*COLUMN_WIDTH  ;i++)
		{
			BUNNY_DBG("-");
		}
		BUNNY_DBG("\r\n");

		for(int i=0; i<NUM_MEM_TESTS; i++)
		{

			mem_test_size[i] = 1<<(10+i);

			bunny_mem__speed_test__blocks(cache_enabled,
										  mem_test_size[i],
										  CONFIG__TEST_ITERATIONS,
										  &mem_test_result[i]);

			snprintf(col_string[0] , MAX_COLUMN_SIZE, "%s%d bytes",column_colors[0], (int)mem_test_size[i]);
			snprintf(col_string[1] , MAX_COLUMN_SIZE ,"%s%.2f MB/s",column_colors[1], CYCLES_TO_RATE(mem_test_result[i].cycle_count__memcpy_read, mem_test_size[i]) );
			snprintf(col_string[2] , MAX_COLUMN_SIZE ,"%s%.2f MB/s",column_colors[2], CYCLES_TO_RATE(mem_test_result[i].cycle_count__dma_read, mem_test_size[i]) );
			snprintf(col_string[3] , MAX_COLUMN_SIZE, "%s%.2f MB/s",column_colors[3], CYCLES_TO_RATE(mem_test_result[i].cycle_count__memcpy_write, mem_test_size[i]) );
			snprintf(col_string[4] , MAX_COLUMN_SIZE ,"%s%.2f MB/s",column_colors[4], CYCLES_TO_RATE(mem_test_result[i].cycle_count__dma_write, mem_test_size[i]) );

			for(int j=0;j<NUM_COLUMNS;j++)
			{
				PRINT_AT_COLUMN(START_COLUMN + (COLUMN_WIDTH   * j),"%s",col_string[j]);
			}


			if(mem_test_size[i]>16384)
			{
				PRINT_AT_COLUMN(START_COLUMN + (COLUMN_WIDTH   * NUM_COLUMNS),VT100_RED"  <<<CACHE CLIFF");
			}

			BUNNY_DBG(VT100_WHITE"\r\n");
		}

		BUNNY_DBG("\r\n");

		#if CONFIG__PSRAM_RANDOM_ACCESS_SPEED_CHECK == 1

			bunny_mem__speed_test__random_addr(false,
											   CONFIG__RANDOM_BLOCK_SIZE,
											   CONFIG__TEST_ITERATIONS,
											   &btr);

			BUNNY_DBG("Random access tests\r\n");
			BUNNY_DBG("-------------------------------------------------------------------------\r\n");
			BUNNY_DBG("\r\n");

			BUNNY_DBG("Within 16KB boundary\r\n");
			BUNNY_DBG("------------------------------------\r\n");

			BUNNY_DBG("Read access            : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__cache_range_read_only,CONFIG__RANDOM_BLOCK_SIZE));
			BUNNY_DBG("Write access           : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__cache_range_write_then_read,CONFIG__RANDOM_BLOCK_SIZE));
			BUNNY_DBG("Write then read access : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__cache_range_write_then_read,CONFIG__RANDOM_BLOCK_SIZE));

			BUNNY_DBG("\r\n");
			BUNNY_DBG("Across %d byte range \r\n",CONFIG__BUNNY_DRAM_SIZE);
			BUNNY_DBG("------------------------------------\r\n");
			BUNNY_DBG("Read access            : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__fullspan_read_only,CONFIG__RANDOM_BLOCK_SIZE));
			BUNNY_DBG("Write access           : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__fullspan_write_only,CONFIG__RANDOM_BLOCK_SIZE));
			BUNNY_DBG("Write then read access : %.2f MB/s\r\n",CYCLES_TO_RATE(btr.cycle_count__fullspan_write_then_read,CONFIG__RANDOM_BLOCK_SIZE));

		#endif

		BUNNY_DBG("\r\n");

	}// for(test_run)

	BUNNY_DBG("\r\n");
	BUNNY_DBG("Test sequence complete\r\n");
	BUNNY_DBG("\r\n");

    while(1)
    {

        __asm volatile ("nop");
    }
    return 0 ;
}
