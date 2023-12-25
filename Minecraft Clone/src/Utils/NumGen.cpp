#include <iostream>
#include "NumGen.h"

long long unsigned int count = 0;

long long unsigned int ID_VAL = 0;

long long unsigned int getID() {
    ID_VAL++;
    return ID_VAL;
}