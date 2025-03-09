#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(FINDER_LIB)
#  define FINDER_EXPORT Q_DECL_EXPORT
# else
#  define FINDER_EXPORT 
# endif
#else
# define FINDER_EXPORT
#endif
