#include "Probability.h"

#include <cstdlib>

bool TestProbability(double odds) {
  int rng = rand();

  if ((static_cast<double>(RAND_MAX)) * odds >= static_cast<double>(rng)) {
    return true;
  }

  return false;
}
