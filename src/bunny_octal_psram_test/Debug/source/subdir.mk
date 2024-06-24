################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/bunny_mem.c \
../source/main.c \
../source/semihost_hardfault.c \
../source/system.c 

C_DEPS += \
./source/bunny_mem.d \
./source/main.d \
./source/semihost_hardfault.d \
./source/system.d 

OBJS += \
./source/bunny_mem.o \
./source/main.o \
./source/semihost_hardfault.o \
./source/system.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\board" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\drivers" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\device" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\utilities" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\uart" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\serial_manager" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\lists" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\CMSIS" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\startup" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\source" -O2 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fsingle-precision-constant -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/bunny_mem.d ./source/bunny_mem.o ./source/main.d ./source/main.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/system.d ./source/system.o

.PHONY: clean-source

