################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/AS1.c \
../Generated_Code/Cpu.c \
../Generated_Code/MISO.c \
../Generated_Code/MOSI.c \
../Generated_Code/NSS.c \
../Generated_Code/PE_LDD.c \
../Generated_Code/RST.c \
../Generated_Code/SCK.c \
../Generated_Code/Vectors.c 

OBJS += \
./Generated_Code/AS1.o \
./Generated_Code/Cpu.o \
./Generated_Code/MISO.o \
./Generated_Code/MOSI.o \
./Generated_Code/NSS.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/RST.o \
./Generated_Code/SCK.o \
./Generated_Code/Vectors.o 

C_DEPS += \
./Generated_Code/AS1.d \
./Generated_Code/Cpu.d \
./Generated_Code/MISO.d \
./Generated_Code/MOSI.d \
./Generated_Code/NSS.d \
./Generated_Code/PE_LDD.d \
./Generated_Code/RST.d \
./Generated_Code/SCK.d \
./Generated_Code/Vectors.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../Sources" -I"../Includes" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/PDD" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/IO_Map" -I"C:/Users/Wei/workspace.kds/FM1702SL/Sources" -I"C:/Users/Wei/workspace.kds/FM1702SL/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


