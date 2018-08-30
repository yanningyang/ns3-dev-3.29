//
// MATLAB Compiler: 6.6 (R2018a)
// Date: Thu Aug 30 15:01:52 2018
// Arguments:
// "-B""macro_default""-W""cpplib:libMADecode""-T""link:lib""MADecode.m"
//

#ifndef __libMADecode_h
#define __libMADecode_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libMADecode_C_API 
#define LIB_libMADecode_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_libMADecode_C_API 
bool MW_CALL_CONV libMADecodeInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libMADecode_C_API 
bool MW_CALL_CONV libMADecodeInitialize(void);

extern LIB_libMADecode_C_API 
void MW_CALL_CONV libMADecodeTerminate(void);

extern LIB_libMADecode_C_API 
void MW_CALL_CONV libMADecodePrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_libMADecode_C_API 
bool MW_CALL_CONV mlxMADecode(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_libMADecode
#define PUBLIC_libMADecode_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libMADecode_CPP_API __declspec(dllimport)
#endif

#define LIB_libMADecode_CPP_API PUBLIC_libMADecode_CPP_API

#else

#if !defined(LIB_libMADecode_CPP_API)
#if defined(LIB_libMADecode_C_API)
#define LIB_libMADecode_CPP_API LIB_libMADecode_C_API
#else
#define LIB_libMADecode_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libMADecode_CPP_API void MW_CALL_CONV MADecode(int nargout, mwArray& flag, mwArray& data, mwArray& cache_curr, const mwArray& cache, const mwArray& solution);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
