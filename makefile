# Setup toolchain
TOOLCHAIN_PREFIX = arm-none-eabi-
CC = $(TOOLCHAIN_PREFIX)gcc
CPP = $(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_PREFIX)g++
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy
SIZE = $(TOOLCHAIN_PREFIX)size
AS = $(TOOLCHAIN_PREFIX)as
RM = cs-rm

LST_FLAGS = -Wa,-aln
C_FLAGS = -Wall -Wextra -O0 -mcpu=cortex-m3 -mthumb -DSTM32F10X_MD_VL -DUSE_STDPERIPH_DRIVER -I$(STDLIBIPATH) -I$(APPSRCPATH) -g -c  
LD_FLAGS = -Wall -O0 -Tlinkerscript.ld -nostdlib -nostartfiles


# Setup paths
PROJECTPATH=
 #D:/Data/Privat/Development/CortexM3/RC_Ctrl
SRCPATH=Src
APPSRCPATH=$(SRCPATH)/Application
STDLIBIPATH=$(SRCPATH)/STD_Libraries/STM32F10x_StdPeriph_Driver/inc
STDLIBSPATH=$(SRCPATH)/STD_Libraries/STM32F10x_StdPeriph_Driver/src

OUTPATH=Out
TMPPATH=Tmp

#Source directories
VPATH = $(STDLIBSPATH):$(APPSRCPATH)

#Files to compile
FILES= \
main.c \
RC_Core.c \
Axises.c \
HAL.c \
stm32f10x_rcc.c \
stm32f10x_gpio.c \
stm32f10x_usart.c \
stm32f10x_adc.c \
stm32f10x_tim.c \
stm32f10x_dma.c \
system_stm32f10x.c \
misc.c

# Autogenerate object files 
tOBJFILES=$(addprefix $(TMPPATH)/,$(FILES))
OBJFILES=$(subst .c,.o,$(tOBJFILES))

all: $(OBJFILES) Startup
	$(CC) $(OBJFILES) $(LD_FLAGS) $(TMPPATH)/Startup.o -o $(OUTPATH)/Application.elf
	$(OBJCPY) -O ihex $(OUTPATH)/Application.elf $(OUTPATH)/Application.hex
	$(SIZE) $(OUTPATH)/Application.elf

$(TMPPATH)/%.o : %.c
	@echo $(STDLIBIPATH)
	@echo -- Compiling $< --
	$(CC) $(C_FLAGS)  $(LST_FLAGS) $< -o $@ > $@.lst

Startup:
	$(CC) $(C_FLAGS) $(LST_FLAGS) $(APPSRCPATH)/startup_stm32f10x_md_vl.s -o $(TMPPATH)/Startup.o > $(TMPPATH)/Startup.lst

# If directory does not exist, make it
$(OBJFILES) : | $(TMPPATH)

$(TMPPATH):
	md $(TMPPATH)

clean:
	$(RM) $(OUTPATH)/* 
	$(RM) $(TMPPATH)/* 
	
	