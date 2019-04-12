BASE = ../../../
FCMAKE = C:/Program Files (x86)/FITkit/bin/fcmake.exe
FKFLASH = C:/Program Files (x86)/FITkit/bin/fkflash.exe
FKTERM = C:/Program Files (x86)/FITkit/bin/fkterm.exe
FKTEST = C:/Program Files (x86)/FITkit/bin/fktest.exe
PROJECT = project.xml
OUTPUTPREFIX = sid
FPGACHIP = xc3s50
FPGASPEEDGRADE = 4
FPGAPACKAGE = pq208

all: dependencycheck build/sid_f1xx.hex build/sid_f2xx.hex build/sid.bin

#MCU part
#=====================================================================
HEXFILE_F1XX = build/sid_f1xx.hex
HEXFILE_F2XX = build/sid_f2xx.hex

build/mcu/audio_codec_f1xx.o: ../../../mcu/libs/codec/audio_codec.c
	$(comp_tpl_f1xx)

build/mcu/audio_codec_f2xx.o: ../../../mcu/libs/codec/audio_codec.c
	$(comp_tpl_f2xx)

build/mcu/main_f1xx.o: mcu/main.c
	$(comp_tpl_f1xx)

build/mcu/main_f2xx.o: mcu/main.c
	$(comp_tpl_f2xx)

OBJFILES_F1XX = build/mcu/audio_codec_f1xx.o build/mcu/main_f1xx.o
OBJFILES_F2XX = build/mcu/audio_codec_f2xx.o build/mcu/main_f2xx.o

#FPGA part
#=====================================================================
BINFILE = build/sid.bin
HDLFILES  = ../../../fpga/units/clkgen/clkgen_config.vhd
HDLFILES += ../../../fpga/units/clkgen/clkgen.vhd
HDLFILES += ../../../fpga/units/math/math_pack.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc_entity.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc_autoincr.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_reg.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_ctrl.vhd
HDLFILES += ../../../fpga/chips/fpga_xc3s50.vhd
HDLFILES += ../../../fpga/chips/architecture_gp/arch_gp_ifc.vhd
HDLFILES += ../../../fpga/chips/architecture_gp/tlv_gp_ifc.vhd
HDLFILES += fpga/engen.vhd
HDLFILES += fpga/top.vhd
HDLFILES += fpga/sid6581.vhd
HDLFILES += fpga/voice_gen.vhd

build/sid.bin: build/fpga/sid.par.ncd build/fpga/sid.pcf

PKGS_LIST = ../../../mcu/libs/codec/package.xml
PKGS_LIST += ../../../fpga/units/clkgen/package.xml
PKGS_LIST += ../../../fpga/units/math/package.xml
PKGS_LIST += ../../../fpga/ctrls/spi/package.xml
PKGS_LIST += ../../../fpga/chips/architecture_gp/package.xml

include $(BASE)/base/Makefile.inc
