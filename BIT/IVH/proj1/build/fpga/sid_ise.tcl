#============================================================================
# Run: 
#    xtclsh sid_ise.tcl  - creates XILINX ISE project file
#    ise sid_project.ise - opens the project
#============================================================================
source "../../../../../base/xilinxise.tcl"

project_new "sid_project"
project_set_props
puts "Adding source files"
xfile add "../../../../../fpga/units/clkgen/clkgen_config.vhd"
xfile add "sid_config.vhd"
xfile add "../../../../../fpga/units/clkgen/clkgen.vhd"
xfile add "../../../../../fpga/units/math/math_pack.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc_entity.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc_autoincr.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_reg.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_ctrl.vhd"
xfile add "../../../../../fpga/chips/fpga_xc3s50.vhd"
xfile add "../../../../../fpga/chips/architecture_gp/arch_gp_ifc.vhd"
xfile add "../../../../../fpga/chips/architecture_gp/tlv_gp_ifc.vhd"
xfile add "../../fpga/engen.vhd"
xfile add "../../fpga/top.vhd"
xfile add "../../fpga/sid6581.vhd"
xfile add "../../fpga/voice_gen.vhd"
puts "Libraries"
project_set_top "fpga"
project_close
