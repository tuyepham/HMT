################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Host/gatt_uuid.obj: C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/host/gatt_uuid.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/config/build_components.opt" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/config/factory_config.opt" --cmd_file="E:/privates/HME/TI/simple_observer_cc2640r2lp_stack_library/TOOLS/build_config.opt"  -mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="E:/privates/HME/TI/simple_observer_cc2640r2lp_stack_library" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/rom" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/common/cc26xx/npi/stack" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHXL/blestack/simple_observer/src/stack" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/profiles/roles" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/hal/src/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/npi/src" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/services/src/aes/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/services/src/nv/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/services/src/nv" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/blestack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2/rf_patches" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/devices/cc26x0r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/source" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages" --include_path="C:/ti/xdctools_3_50_02_20_core/packages" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=CC26XX --define=CC26XX_R2 --define=DeviceFamily_CC26X0R2 --define=EXT_HAL_ASSERT --define=FLASH_ROM_BUILD --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=OSAL_CBTIMER_NUM_TASKS=1 --define=OSAL_SNV=1 --define=POWER_SAVING --define=RF_SINGLEMODE --define=STACK_LIBRARY --define=USE_ICALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="Host/gatt_uuid.d_raw" --obj_directory="Host" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


