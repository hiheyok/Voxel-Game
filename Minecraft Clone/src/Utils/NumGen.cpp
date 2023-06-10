#include <iostream>
#include "NumGen.h"

long long unsigned int count = 0;

long long int getRandomNumber() {

    count++;

    srand((long long unsigned int)time(NULL) + count);

    return (long long int)((float)rand() / (float)RAND_MAX);
}

long long unsigned int ID_VAL = 0;

long long unsigned int getID() {
    ID_VAL++;
    return ID_VAL;
}