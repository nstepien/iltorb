/* force mempcy to be from earlier compatible system */
/* see https://github.com/MayhemYDG/iltorb/issues/35 */
__asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
