#ifndef TYPES_H
#define TYPES_H

#include <cstring>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "common.h"
/* User defined types */
typedef unsigned int uint;
namespace fs = std::filesystem;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure,
    d_success,
    d_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif