/**
 * Author: caterpillow
 * Date: 2025-10-24
 * License: CC0
 * Source: caterpillow
 * Description: tspmo
 * Status: tested
 */

#pragma once

#ifdef __cplusplus
extern "C"
#endif
const char* __asan_default_options() { return "detect_leaks=0"; }