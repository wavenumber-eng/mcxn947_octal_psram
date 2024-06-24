################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\board" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\drivers" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\device" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\utilities" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\uart" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\serial_manager" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\component\lists" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\CMSIS" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\startup" -I"C:\ELI\projects\Wavenumber\BunnyVision\bunny_brain\test\psram_test\source" -O2 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fsingle-precision-constant -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-board

clean-board:
	-$(RM) ./board/board.d ./board/board.o ./board/clock_config.d ./board/clock_config.o ./board/peripherals.d ./board/peripherals.o ./board/pin_mux.d ./board/pin_mux.o

.PHONY: clean-board

