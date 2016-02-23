################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Project_Settings/Startup_Code/startup.c \
../Project_Settings/Startup_Code/system_MKL04Z4.c 

S_UPPER_SRCS += \
../Project_Settings/Startup_Code/startup_MKL04Z4.S 

OBJS += \
./Project_Settings/Startup_Code/startup.o \
./Project_Settings/Startup_Code/startup_MKL04Z4.o \
./Project_Settings/Startup_Code/system_MKL04Z4.o 

C_DEPS += \
./Project_Settings/Startup_Code/startup.d \
./Project_Settings/Startup_Code/system_MKL04Z4.d 

S_UPPER_DEPS += \
./Project_Settings/Startup_Code/startup_MKL04Z4.d 


# Each subdirectory must supply rules for building sources it contributes
Project_Settings/Startup_Code/%.o: ../Project_Settings/Startup_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../Sources" -I"../Includes" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/PDD" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/IO_Map" -I"C:/Users/Wei/workspace.kds/FM1702SL/Sources" -I"C:/Users/Wei/workspace.kds/FM1702SL/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Project_Settings/Startup_Code/%.o: ../Project_Settings/Startup_Code/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


