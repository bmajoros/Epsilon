// ============================================
// Random.H
// ============================================

#ifndef INCL_RANDOM_H
#define INCL_RANDOM_H


const int LARGEST_RANDOM_NUMBER=0xFFFF;


// randoize() ensures that random numbers are unique across
// program invocations
void randomize();


// RandomNumber returns a random integer between 0 and n-1
// (inclusive):
int RandomNumber(int n);

// Random0to1() returns a random floating-point number in
// the interval [0,1]:
float Random0to1();

#endif
