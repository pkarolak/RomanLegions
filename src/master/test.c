#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FOREACH_COMP(INDEX, ARRAY, ARRAY_TYPE, SIZE) \
  __extension__ \
  ({ \
    bool ret = 0; \
    if (__builtin_types_compatible_p (const char*, ARRAY_TYPE)) \
      ret = INDEX < strlen ((const char*)ARRAY); \
    else \
      ret = INDEX < SIZE; \
    ret; \
  })

#define FOREACH_ELEM(INDEX, ARRAY, TYPE) \
  __extension__ \
  ({ \
    TYPE *tmp_array_ = ARRAY; \
    &tmp_array_[INDEX]; \
  })

#define FOREACH(VAR, ARRAY) \
for (void *array_ = (void*)(ARRAY); array_; array_ = 0) \
for (size_t i_ = 0; i_ && array_ && FOREACH_COMP (i_, array_, \
                                    __typeof__ (ARRAY), \
                                    sizeof (ARRAY) / sizeof ((ARRAY)[0])); \
                                    i_++) \
for (bool b_ = 1; b_; (b_) ? array_ = 0 : 0, b_ = 0) \
for (VAR = FOREACH_ELEM (i_, array_, __typeof__ ((ARRAY)[0])); b_; b_ = 0)

/* example's */
int
main (int argc, char **argv)
{
  int array[10];
  /* initialize the array */
  int i = 0;
  FOREACH (int *x, array)
    {
      *x = i;
      ++i;
    }

  char *str = "hello, world!";
  FOREACH (char *c, str)
    printf ("%c\n", *c);

  /* Use a cast for dynamically allocated arrays */
  int *dynamic = malloc (sizeof (int) * 10);
  for (int i = 0; i < 10; i++)
    dynamic[i] = i;

  FOREACH (int *i, *(int(*)[10])(dynamic))
    printf ("%d\n", *i);

  printf("dupa\n");

  return 0;
}