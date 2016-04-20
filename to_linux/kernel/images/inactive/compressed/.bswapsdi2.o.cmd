cmd_arch/arm/boot/compressed/bswapsdi2.o := arm-linux-gnueabihf-gcc -Wp,-MD,arch/arm/boot/compressed/.bswapsdi2.o.d  -nostdinc -isystem /home/korena/Development/Tools/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf/bin/../lib/gcc/arm-linux-gnueabihf/5.1.1/include -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/include/uapi -Iinclude/generated/uapi -include /home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-s5pv210/include -Iarch/arm/plat-samsung/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -Wa,--gdwarf-2     -DZIMAGE     -c -o arch/arm/boot/compressed/bswapsdi2.o arch/arm/boot/compressed/bswapsdi2.S

source_arch/arm/boot/compressed/bswapsdi2.o := arch/arm/boot/compressed/bswapsdi2.S

deps_arch/arm/boot/compressed/bswapsdi2.o := \
  /home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/korena/Development/Workspaces/car-dashboard/to_linux/kernel/linux-3.14.61/arch/arm/include/asm/linkage.h \

arch/arm/boot/compressed/bswapsdi2.o: $(deps_arch/arm/boot/compressed/bswapsdi2.o)

$(deps_arch/arm/boot/compressed/bswapsdi2.o):
