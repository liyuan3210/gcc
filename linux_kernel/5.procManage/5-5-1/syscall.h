#ifndef _MYSYSCALL_
#define _MYSYSCALL_

#define __NR_getjiffies     370
#define __NR_diffjiffies    371
#define __NR_pdiffjiffies   372

inline long getjiffies(void)
{
    return syscall(__NR_getjiffies);
}

inline long diffjiffies(long ujiffies)
{
    return syscall(__NR_diffjiffies, ujiffies);
}

inline long pdiffjiffies(long ujiffies, long* presult)
{
    return syscall(__NR_pdiffjiffies, ujiffies, presult);
}
#endif
