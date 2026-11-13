#ifndef COLD_BOOT_COMMON_H
#define COLD_BOOT_COMMON_H

#include <stddef.h>
#include <stdint.h>

#include "mhu.h"

/* Tune against the core clock for the board/bitstream under test. */
#define COLD_BOOT_TIMEOUT_CYCLES 100000000u
#define COLD_BOOT_SPIN_LIMIT     50000000u

#define COLD_BOOT_M85_READY     (1u << 0)
#define COLD_BOOT_M85_DATA_OK   (1u << 1)
#define COLD_BOOT_R5_ACK        (1u << 2)
#define COLD_BOOT_ALL_OK        (1u << 3)

static uint32_t cold_boot_cycles(void);

static int cold_boot_wait_bits(volatile const uint32_t *reg, uint32_t mask,
                               int cycles_ok)
{
    uint32_t start = cold_boot_cycles();

    for (uint32_t spins = 0u; spins < COLD_BOOT_SPIN_LIMIT; ++spins) {
        if ((*reg & mask) == mask)
            return 1;
        if (cycles_ok &&
            (uint32_t)(cold_boot_cycles() - start) >=
                COLD_BOOT_TIMEOUT_CYCLES)
            return 0;
    }

    return 0;
}

static uint32_t cold_boot_hash_words(volatile const uint32_t *data,
                                     size_t count)
{
    uint32_t hash = 2166136261u;

    for (size_t i = 0u; i < count; ++i)
        hash = (hash ^ data[i]) * 16777619u;

    return hash;
}

static int cold_boot_region_contains(uintptr_t base, size_t region_size,
                                     uintptr_t address, size_t object_size)
{
    return address >= base && object_size <= region_size &&
           address - base <= region_size - object_size;
}

static int cold_boot_image_ok(uintptr_t entry,
                              uintptr_t code_base,
                              size_t code_size,
                              volatile uint32_t *payload,
                              size_t payload_words,
                              uint32_t expected_hash,
                              volatile uint32_t *write_probe,
                              uint32_t initial_probe,
                              uintptr_t data_base,
                              size_t data_size)
{
    uintptr_t code_address = entry & ~(uintptr_t)1u;
    int data_ok = cold_boot_hash_words(payload, payload_words) ==
                  expected_hash;
    int data_in_tcm =
        cold_boot_region_contains(data_base, data_size, (uintptr_t)payload,
                                  payload_words * sizeof(*payload)) &&
        cold_boot_region_contains(data_base, data_size,
                                  (uintptr_t)write_probe,
                                  sizeof(*write_probe));

    data_ok = data_ok && (*write_probe == initial_probe);
    *write_probe = ~initial_probe;
    data_ok = data_ok && (*write_probe == ~initial_probe);
    *write_probe = initial_probe;

    return cold_boot_region_contains(code_base, code_size, code_address, 1u) &&
           data_in_tcm && data_ok;
}

#endif
