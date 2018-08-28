//
// MATLAB Compiler: 6.6 (R2018a)
// Date: Tue Aug 28 14:44:24 2018
// Arguments: "-B""macro_default""-W""cpplib:libMA""-T""link:lib""MA.m"
//

#ifndef __libMA_h
#define __libMA_h 1

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
#ifndef LIB_libMA_C_API 
#define LIB_libMA_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_libMA_C_API 
bool MW_CALL_CONV libMAInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libMA_C_API 
bool MW_CALL_CONV libMAInitialize(void);

extern LIB_libMA_C_API 
void MW_CALL_CONV libMATerminate(void);

extern LIB_libMA_C_API 
void MW_CALL_CONV libMAPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_libMA_C_API 
bool MW_CALL_CONV mlxMA(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_libMA
#define PUBLIC_libMA_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libMA_CPP_API __declspec(dllimport)
#endif

#define LIB_libMA_CPP_API PUBLIC_libMA_CPP_API

#else

#if !defined(LIB_libMA_CPP_API)
#if defined(LIB_libMA_C_API)
#define LIB_libMA_CPP_API LIB_libMA_C_API
#else
#define LIB_libMA_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libMA_CPP_API void MW_CALL_CONV MA(int nargout, mwArray& solution, mwArray& TIME, const mwArray& V_MATRIX, const mwArray& TICK);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
