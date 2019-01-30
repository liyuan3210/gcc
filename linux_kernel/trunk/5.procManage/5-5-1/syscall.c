#include <stdio.h>
#include <unistd.h>
#include "syscall.h"

int main(void)
{
    long jifs, result;
    int err;

    jifs = getjiffies();
    sleep(1);
    printf("difference is %ld\n", diffjiffies(jifs));
    sleep(1);
    err = pdiffjiffies(jifs, &result);

    if (!err) {
        printf("difference is %ld\n", result);
    } else {
        printf("error\n");
    }
    return 0;
}
