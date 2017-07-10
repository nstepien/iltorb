/* force mempcy to be from earlier compatible system */
/* see https://github.com/MayhemYDG/iltorb/issues/35 */
/*     https://github.com/MayhemYDG/iltorb/issues/39 */
/*     https://github.com/MayhemYDG/iltorb/issues/40 */
#if defined(__linux__) && defined(__GLIBC__) && defined(__x86_64__)
  __asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
#endif
