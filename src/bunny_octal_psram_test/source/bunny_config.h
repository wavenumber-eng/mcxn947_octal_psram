
#ifndef BUNNY_CONFIG_H_
#define BUNNY_CONFIG_H_


/*
*	This this will write blocks of randomized data across
*	the entire PSRAM array then readback & verify.
*
*/

#define CONFIG__PSRAM_MEM_TEST                  (1)

 /*
  *
  * Enables the test suite for doing reads/writes to
  * random addressed over a large address range
  *
  */
#define CONFIG__PSRAM_RANDOM_ACCESS_SPEED_CHECK	(1)

 /*
  *
  * Routes the tests to a small chunk of internal memory to
  * establish a baseline of performance to use for comparison.
  *
  */

#define CONFIG__PSRAM_SPOOF  					(0)

/*
 * This is the max block size to reserve for testing.  We are using memory liberally
 * for the tests, so it really can't get mach larger
 *
 */

#define CONFIG__MAX_BLOCK_SIZE     				(32768)

/*
 * This is the max block size for the random read writes.  It is smaller than
 * the blocks used for DMA/memcpy as we also store a pre-computed list of address
 *
 */

#define	CONFIG__RANDOM_BLOCK_SIZE				(1024)

/*
 * The number of timers a test will run.
 * Cycles counts will be averaged over the iterations
 */
#define CONFIG__TEST_ITERATIONS					(256)


/*
 * When we swap in the internal ram to be a placeholder for PSRAM,
 * we need to shrink the max address range for the tests.   PSRAM is large.
 *
 */

#if CONFIG__PSRAM_SPOOF == 1
	#define   CONFIG__BUNNY_DRAM_SIZE 				 CONFIG__MAX_BLOCK_SIZE
#else
	#define   CONFIG__BUNNY_DRAM_SIZE 				 (0x800000U)
#endif


#endif /* BUNNY_CONFIG_H_ */
