################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_UPPER_SRCS += \
../Sources/ISO14443A.C \
../Sources/SlRc1702.C 

C_SRCS += \
../Sources/Events.c \
../Sources/main.c \
../Sources/main_backup.c 

OBJS += \
./Sources/Events.o \
./Sources/ISO14443A.o \
./Sources/SlRc1702.o \
./Sources/main.o \
./Sources/main_backup.o 

C_DEPS += \
./Sources/Events.d \
./Sources/main.d \
./Sources/main_backup.d 

C_UPPER_DEPS += \
./Sources/ISO14443A.d \
./Sources/SlRc1702.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../Sources" -I"../Includes" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/PDD" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/IO_Map" -I"C:/Users/Wei/workspace.kds/FM1702SL/Sources" -I"C:/Users/Wei/workspace.kds/FM1702SL/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/%.o: ../Sources/%.C
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../Sources" -I"../Includes" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/PDD" -I"C:/Users/Wei/workspace.kds/FM1702SL/Static_Code/IO_Map" -I"C:/Users/Wei/workspace.kds/FM1702SL/Sources" -I"C:/Users/Wei/workspace.kds/FM1702SL/Generated_Code" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


