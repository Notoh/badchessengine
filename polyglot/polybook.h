#ifndef BADCHESSENGINE_POLYBOOK_H
#define BADCHESSENGINE_POLYBOOK_H

#include "../defs.h"

#ifdef _MSC_VER
#define U64(u) (u##ui64)
#else
#define U64(u) (u##ULL)
#endif

extern const u64 Random64Poly[781];


#endif //BADCHESSENGINE_POLYBOOK_H
