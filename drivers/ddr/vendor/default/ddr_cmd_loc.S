/* DDR Training command codes location. Copy from DDR to SRAM to train DDR. */

.section .image,#alloc

.globl g_ddr_training_cmd_start
g_ddr_training_cmd_start:
.incbin "drivers/ddr/vendor/default/cmd_bin/ddr_cmd.bin"
.globl g_ddr_training_cmd_end
g_ddr_training_cmd_end:
