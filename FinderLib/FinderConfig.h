#pragma once
#include "FinderGlobal.h"

//#define USE_INDEXER

#define DUMP_WHEN_ONE_FOUND
#define DUMP_WHEN_ALL_FOUND

// selects which indexer to use
const int use_indexer = 1;

// selects walk algorithm to use
const int use_walker = 0;

// selects whether to use QDir::match() or some other algorithm for string matching
const bool use_qdirmatch = true;

// selects a match algorithm alternative when use_qdirmatch = false
const int use_matcher = 2;
