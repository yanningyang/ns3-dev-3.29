//
// MATLAB Compiler: 6.6 (R2018a)
// Date: Mon Aug 27 21:48:10 2018
// Arguments: "-B""macro_default""-W""cpplib:libadd""-T""link:lib""add.m"
//

#ifndef __libadd_h
#define __libadd_h 1

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
#ifndef LIB_libadd_C_API 
#define LIB_libadd_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_libadd_C_API 
bool MW_CALL_CONV libaddInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libadd_C_API 
bool MW_CALL_CONV libaddInitialize(void);

extern LIB_libadd_C_API 
void MW_CALL_CONV libaddTerminate(void);

extern LIB_libadd_C_API 
void MW_CALL_CONV libaddPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_libadd_C_API 
bool MW_CALL_CONV mlxAdd(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_libadd
#define PUBLIC_libadd_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libadd_CPP_API __declspec(dllimport)
#endif

#define LIB_libadd_CPP_API PUBLIC_libadd_CPP_API

#else

#if !defined(LIB_libadd_CPP_API)
#if defined(LIB_libadd_C_API)
#define LIB_libadd_CPP_API LIB_libadd_C_API
#else
#define LIB_libadd_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libadd_CPP_API void MW_CALL_CONV add(int nargout, mwArray& C, const mwArray& A, const mwArray& B);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
