// General type definitions for portability
//
// The following types are usable under both MSVC for Win32 and 
// GCC for Atmel 

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifndef US_TYPEDEFS
#define US_TYPEDEFS

#ifndef _WINDOWS_
#ifdef __cplusplus
typedef bool             BOOL;
#else
typedef unsigned char    BOOL;
#endif
#endif
typedef signed   long    BOOL32;
typedef unsigned char    U8 ;
typedef unsigned short   U16;
typedef unsigned long    U32;
typedef          char    C8 ;
typedef signed   char    S8 ;
typedef signed   short   S16;
typedef signed   long    S32;

#ifdef __AVR__
typedef unsigned long long U64;
typedef long long          S64;
#else
#ifdef __WATCOMC__
typedef unsigned __int64 U64;
typedef signed __int64   S64;
#else
typedef unsigned __int64   U64;
typedef   signed __int64   S64;
#endif
#endif

#endif

#ifndef F_TYPEDEFS
#define F_TYPEDEFS

typedef float           SINGLE;
typedef double          DOUBLE;

#endif

#ifndef YES
#define YES 1
#endif

#ifndef NO
#define NO  0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#endif
