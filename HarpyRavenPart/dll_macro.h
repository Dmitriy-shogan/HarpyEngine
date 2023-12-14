#pragma once
#ifndef HARPY_RAVEN_DLL_MACRO
#define HARPY_RAVEN_DLL_MACRO

#ifdef HARPY_RAVEN_DLL_EXPORTS
#define HARPY_RAVEN_API __declspec(dllexport)
#else
#define HARPY_RAVEN_API __declspec(dllimport)
#endif

#endif//HARPY_RAVEN_DLL_MACRO