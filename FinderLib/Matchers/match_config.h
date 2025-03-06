#pragma once
#include "../FinderConfig.h"

// enable dotglob: *. ?, and [] match a . (dotfile) at the begin or after each /
#define DOTGLOB 1

// enable case-insensitive glob matching
#define NOCASEGLOB 1

#define CASE(c) (NOCASEGLOB ? tolower(c) : (c))

// path separator
#ifdef _WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif