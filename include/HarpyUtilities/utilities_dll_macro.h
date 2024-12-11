#pragma once
#ifndef HARPY_UTILITIES_DLL_MACRO
#define HARPY_UTILITIES_DLL_MACRO

namespace ashvardanian::stringzilla {};
namespace sz = ashvardanian::stringzilla;

#ifdef HARPY_UTILITIES_EXPORTS
#define HARPY_UTILITIES_API __declspec(dllexport)
#else
#define HARPY_UTILITIES_API
#endif

#endif//HARPY_UTILITIES_DLL_MACRO