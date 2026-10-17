#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

<<<<<<< HEAD
  // your code here

=======
>>>>>>> 9d94acfbd6650ad202d89fb7951fcb2dc04bb328
  for (unsigned int i = begin; i < end; i++) {
    if (array[i] < min_max.min) min_max.min = array[i];
    if (array[i] > min_max.max) min_max.max = array[i];
  }
<<<<<<< HEAD
  
=======
>>>>>>> 9d94acfbd6650ad202d89fb7951fcb2dc04bb328
  return min_max;
}
