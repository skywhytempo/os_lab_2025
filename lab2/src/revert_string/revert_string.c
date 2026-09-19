<<<<<<< HEAD
#include "revert_string.h"
#include <string.h>

=======
>>>>>>> 93a8fa8dc937ef2107baae71e856525f78bbd934
void RevertString(char *str)
{
	int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

