// ============================================
// Random.C
// ============================================

#include "Random.H"
#include <stdlib.h>
#include <time.h>
#include <limits.h>


void randomize()
{
  // This function randomizes the random number generator,
  // so that it does not generate the same sequence of
  // pseudo-random numbers for every invocation of this
  // program.
 
  unsigned seed=(unsigned) time(0);
  srand(seed);
}


int RandomNumber(int n)
{
  // returns a random number between 0 and n-1 (inclusive)
  
  return rand() % n;
}



float Random0to1()
{
  // Generate a random number between 1 and maxRand-1, inclusive
  const int randInt=RandomNumber(LARGEST_RANDOM_NUMBER);

  return randInt/(float)(LARGEST_RANDOM_NUMBER-1);
}



