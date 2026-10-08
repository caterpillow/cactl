/**
 * Author: caterpillow
 * Date: 2025-10-24
 * License: CC0
 * Source: caterpillow
 * Description: Disables the leak check under \texttt{-fsanitize=address}.
 * Status: tested
 */

#pragma once

#ifdef __cplusplus
extern "C"
#endif
const char* __asan_default_options() { return "detect_leaks=0"; }