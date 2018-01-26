#ifndef BOOTLOADER_ASSERT_H
#define BOOTLOADER_ASSERT_H

//#define kassert(exp) ((void)((exp)?0:kassert_fail(#exp,__FILE__,__LINE__)))

#define kassert(exp) if (!(exp)) {kassert_fail(#exp, __FILE__, __LINE__);asm volatile("hlt");} // Issue the "hlt" inside the macro so that we stop where the failure happens

void kassert_fail(const char* exp, const char* file, unsigned line);

#endif
