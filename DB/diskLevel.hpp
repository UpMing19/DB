#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include "run.hpp"
#include "diskRun.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cassert>
#include <algorithm>

#define LEFTCHILD(x) x*2+1
#define RIGHTCHILD(x) x*2+2
#define PARENT(x) (x-1)/2

using namespace std;

int TOMBSTONE = INT_MIN;

template<class K,class V>
class DiskLevel{
    


};