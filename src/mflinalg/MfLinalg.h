#pragma once
// mflinalg.dll exports every class; the importing DLLs see the same declarations
// with __declspec(dllimport) (see src/MfActorLib/MfVector3.h).
#define MFLINALG_API __declspec(dllexport)
