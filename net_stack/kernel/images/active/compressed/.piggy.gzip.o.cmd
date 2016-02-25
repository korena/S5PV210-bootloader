cmd_arch/arm/boot/compressed/piggy.gzip.o := arm-linux-gnueabihf-gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem /home/korena/Development/Tools/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf/bin/../lib/gcc/arm-linux-gnueabihf/5.1.1/include -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/include/uapi -Iinclude/generated/uapi -include /home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-s5pv210/include -Iarch/arm/plat-samsung/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -Wa,--gdwarf-2     -DZIMAGE     -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S

source_arch/arm/boot/compressed/piggy.gzip.o := arch/arm/boot/compressed/piggy.gzip.S

deps_arch/arm/boot/compressed/piggy.gzip.o := \
  /home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

arch/arm/boot/compressed/piggy.gzip.o: $(deps_arch/arm/boot/compressed/piggy.gzip.o)

$(deps_arch/arm/boot/compressed/piggy.gzip.o):
