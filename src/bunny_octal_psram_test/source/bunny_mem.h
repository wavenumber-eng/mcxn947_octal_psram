#include <stdbool.h>
#include <stdint.h>
#include "bunny_config.h"
#include "bunny_debug.h"
#include "fsl_common.h"

#ifndef   BUNNY_MEM_H
#define   BUNNY_MEM_H


status_t bunny_mem__init(bool enable_cache);

bool bunny_mem__test(uint32_t block_size, uint32_t iterations);

typedef struct
{
	uint32_t block_size;
	uint32_t iterations;


    uint32_t cycle_count__dma_write;
    uint32_t cycle_count__dma_read;
    uint32_t cycle_count__memcpy_write;
    uint32_t cycle_count__memcpy_read;
    uint32_t cycle_count__cached_dma_write;
    uint32_t cycle_count__cached_dma_read;
    uint32_t cycle_count__cached_memcpy_write;
    uint32_t cycle_count__cached_memcpy_read;

} bunny_mem_speed_test_blocks__result__t;

typedef struct
{
	uint32_t block_size;
	uint32_t iterations;


    uint32_t cycle_count__fullspan_write_then_read;
    uint32_t cycle_count__cache_range_write_then_read;

    uint32_t cycle_count__fullspan_read_only;
    uint32_t cycle_count__cache_range_read_only;

    uint32_t cycle_count__fullspan_write_only;
    uint32_t cycle_count__cache_range_write_only;

} bunny_mem_speed_test_random_access__result__t;



void bunny_mem__speed_test__blocks(bool enable_cache,
						   uint32_t test_block_size,
						   uint32_t iterations,
						   bunny_mem_speed_test_blocks__result__t *bmtr);

void bunny_mem__speed_test__random_addr(bool enable_cache,
										    uint32_t test_block_size,
											uint32_t iterations,
											bunny_mem_speed_test_random_access__result__t  *btr);

#endif
