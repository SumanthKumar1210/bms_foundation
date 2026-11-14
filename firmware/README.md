# BMS firmware (C)

Embedded C application for a 12s lithium-ion battery pack: cell monitoring,
protection faults, passive balancing, contactor sequencing and CAN reporting.
The Python modules in `utils/` are the algorithm prototypes; this folder is the
target implementation.

## Layout

| Path | Content |
|---|---|
| `include/bms/` | Public headers, one per module |
| `src/` | Module implementations |
| `test/` | Host unit tests, a minimal test framework (`unit_test.h`) and the HAL stub |

| Module | Responsibility |
|---|---|
| `bms_types.h` | Status codes and fixed-point unit types (mV, 0.1 degC, mA, ms) |
| `bms_config.h` | Pack layout and every protection, balancing, contactor and CAN limit |
| `hal.h` | Hardware abstraction (ADC, contactor coils, balancing, CAN, timer) |
| `cell_monitor` | Reads cells/sensors, plausibility checks, min/max/sum |
| `fault_manager` | Debounced, latching OV/UV/OT/UT/sensor faults |
| `balancing` | Passive balancing mask |
| `contactor` | Precharge / close / open state machine |
| `can_protocol` | Status and multiplexed cell voltage frames, CRC protected |
| `crc8` | CRC-8 SAE J1850 |
| `bms_app` | Initialisation and the 10 ms main task |

## Coding rules

- C99, no compiler extensions, warnings are errors (see `CMakeLists.txt`).
- Fixed-width types from `bms_types.h`; units in the type or name (`_mv`, `_ms`).
- Every function that can fail returns `bms_status_t`; pointer arguments are
  checked for `NULL`.
- No dynamic memory, no recursion. Module state is `static` in its `.c` file.
- Limits live in `bms_config.h`, never as literals in a module.
- Public functions are prefixed with the module name (`fault_manager_update`).
- New sources must be added to `CMakeLists.txt`; new tests to `BMS_TESTS`.

## Build and test (host)

```bash
cmake -S firmware -B build && cmake --build build && ctest --test-dir build --output-on-failure
```
