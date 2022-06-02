#ifndef MODES_H
#define MODES_H

#include <stdio.h>

void measureInvocationPerformance(unsigned long long *addrStart,
                                  unsigned int startPointCount,
                                  unsigned long long *addrEnd,
                                  unsigned int endPointCount);

void measureGlobalPerformance(unsigned int timeout, float rate);

#endif /* MODES_H */
