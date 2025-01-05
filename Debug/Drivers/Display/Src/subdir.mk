################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Display/Src/ssd1306.c \
../Drivers/Display/Src/ssd1306_fonts.c 

OBJS += \
./Drivers/Display/Src/ssd1306.o \
./Drivers/Display/Src/ssd1306_fonts.o 

C_DEPS += \
./Drivers/Display/Src/ssd1306.d \
./Drivers/Display/Src/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Display/Src/%.o Drivers/Display/Src/%.su Drivers/Display/Src/%.cyclo: ../Drivers/Display/Src/%.c Drivers/Display/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F105xC -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/aymanalamayri/Documents/GitHub/SR16-ChargingDash/Drivers/Display/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Display-2f-Src

clean-Drivers-2f-Display-2f-Src:
	-$(RM) ./Drivers/Display/Src/ssd1306.cyclo ./Drivers/Display/Src/ssd1306.d ./Drivers/Display/Src/ssd1306.o ./Drivers/Display/Src/ssd1306.su ./Drivers/Display/Src/ssd1306_fonts.cyclo ./Drivers/Display/Src/ssd1306_fonts.d ./Drivers/Display/Src/ssd1306_fonts.o ./Drivers/Display/Src/ssd1306_fonts.su

.PHONY: clean-Drivers-2f-Display-2f-Src

