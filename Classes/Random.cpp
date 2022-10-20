//
// Created by Lumpros on 10/2/2022.
//

#include "Random.h"

#include <stdlib.h>

int Random::randInt(int min, int max)
{
    return randInt(max - min) + min;
}

int Random::randInt(int max)
{
    return rand() % (max + 1);
}