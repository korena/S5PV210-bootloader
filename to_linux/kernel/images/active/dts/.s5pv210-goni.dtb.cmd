cmd_arch/arm/boot/dts/s5pv210-goni.dtb := mkdir -p arch/arm/boot/dts/ ; arm-linux-gnueabihf-gcc -E -Wp,-MD,arch/arm/boot/dts/.s5pv210-goni.dtb.d.pre.tmp -nostdinc -I./arch/arm/boot/dts -I./arch/arm/boot/dts/include -I./drivers/of/testcase-data -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.s5pv210-goni.dtb.dts.tmp arch/arm/boot/dts/s5pv210-goni.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/s5pv210-goni.dtb -b 0 -i arch/arm/boot/dts/  -d arch/arm/boot/dts/.s5pv210-goni.dtb.d.dtc.tmp arch/arm/boot/dts/.s5pv210-goni.dtb.dts.tmp ; cat arch/arm/boot/dts/.s5pv210-goni.dtb.d.pre.tmp arch/arm/boot/dts/.s5pv210-goni.dtb.d.dtc.tmp > arch/arm/boot/dts/.s5pv210-goni.dtb.d

source_arch/arm/boot/dts/s5pv210-goni.dtb := arch/arm/boot/dts/s5pv210-goni.dts

deps_arch/arm/boot/dts/s5pv210-goni.dtb := \
  arch/arm/boot/dts/include/dt-bindings/input/input.h \
  arch/arm/boot/dts/include/dt-bindings/input/linux-event-codes.h \
  arch/arm/boot/dts/s5pv210.dtsi \
  arch/arm/boot/dts/skeleton.dtsi \
  arch/arm/boot/dts/include/dt-bindings/clock/s5pv210.h \
  arch/arm/boot/dts/include/dt-bindings/clock/s5pv210-audss.h \
  arch/arm/boot/dts/s5pv210-pinctrl.dtsi \

arch/arm/boot/dts/s5pv210-goni.dtb: $(deps_arch/arm/boot/dts/s5pv210-goni.dtb)

$(deps_arch/arm/boot/dts/s5pv210-goni.dtb):
