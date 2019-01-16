##### Makefile for the ESS deliverables #####
# DO NOT CHANGE THIS FILE, IT WILL BE REPLACED BY THE ORIGINAL VERSION BEFORE GRADING #

include student.mk

# Compiler
CROSS_COMPILE=arm-none-eabi

CC = $(CROSS_COMPILE)-gcc --specs=nosys.specs
LD = $(CROSS_COMPILE)-gcc --specs=nosys.specs
AR = $(CROSS_COMPILE)-ar
AS = $(CROSS_COMPILE)-as
OC = $(CROSS_COMPILE)-objcopy
OD = $(CROSS_COMPILE)-objdump
SZ = $(CROSS_COMPILE)-size
ifeq ($(GDB_QUIET),true)
  DB = $(CROSS_COMPILE)-gdb -quiet
else
  DB = $(CROSS_COMPILE)-gdb
endif

# Paths to build directories for out-of-tree build and path to LUFA USB library
BUILDDIR = build
LIB_BUILDDIR = lib_build
USB_LIBDIR = $(XMC_LIBDIR)/ThirdPartyLibraries/USBlib/USB

# Specify device
XMC_SERIES=4500
XMC_PACKAGE=F100
XMC_SIZE=1024

VPATH += $(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/BasicMathFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/CommonTables:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/ComplexMathFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/ControllerFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/FastMathFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/FilteringFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/MatrixFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/StatisticsFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/SupportFunctions:$(XMC_LIBDIR)/CMSIS/DSP_Lib/Source/TransformFunctions:$(XMC_LIBDIR)/CMSIS/Infineon/XMC$(XMC_SERIES)_series/Source:$(XMC_LIBDIR)/CMSIS/RTOS/RTX/SRC:$(XMC_LIBDIR)/CMSIS/RTOS/RTX/SRC/ARM:$(XMC_LIBDIR)/CMSIS/RTOS/RTX/Templates:$(XMC_LIBDIR)/CMSIS/RTOS/RTX/UserCodeTemplates:$(XMC_LIBDIR)/ThirdPartyLibraries/Newlib:$(XMC_LIBDIR)/XMCLib/src:$(USB_LIBDIR)/Class/Common:$(USB_LIBDIR)/Class/Device:$(USB_LIBDIR)/Common:$(USB_LIBDIR)/Core:$(USB_LIBDIR)/Core/XMC4000

# Where to find CMSIS, device and XMClib header files
CFLAGS  = -I$(XMC_LIBDIR)/CMSIS/Include/
# Where to find device specific header files
CFLAGS += -I$(XMC_LIBDIR)/CMSIS/Infineon/XMC$(XMC_SERIES)_series/Include/
# Where to find XMClib headers
CFLAGS += -I$(XMC_LIBDIR)/XMCLib/inc/
# USB LIB Include paths
CFLAGS += -I$(USB_LIBDIR)
CFLAGS += -I$(USB_LIBDIR)/Class
CFLAGS += -I$(USB_LIBDIR)/Class/Common
CFLAGS += -I$(USB_LIBDIR)/Class/Device
CFLAGS += -I$(USB_LIBDIR)/Common
CFLAGS += -I$(USB_LIBDIR)/Core
CFLAGS += -I$(USB_LIBDIR)/Core/XMC4000
# Which device
CFLAGS += -DXMC$(XMC_SERIES)_$(XMC_PACKAGE)x$(XMC_SIZE)
# Which core architecture
CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb
# Debug level and format
CFLAGS += -g3 -gdwarf-2
# Compile without linking
CFLAGS += -c
# Write separate *.lst file for each source
CFLAGS += -Wa,-adhlns="$@.lst"
# Until here all options are also necessary for compiling startup_XMC4500.S
SFLAGS = $(CFLAGS)
# Add student options, so -Wall comes after and overrides any possible -Wnone or so
CFLAGS +=$(SCFLAGS)
# Enable standard warnings for both
SFLAGS += -Wall
CFLAGS += -Wall
# Put functions into separate sections, so they can be omitted by linker if unused
CFLAGS += -ffunction-sections
# Option for assembly compilation
SFLAGS += -x assembler-with-cpp
# Tell the linker to use our own linker script and startup files
LFLAGS  = -T$(BUILDDIR)/$(LD_NAME).ld -nostartfiles
# Where to find the precompiled math libraries
LFLAGS += -L$(XMC_LIBDIR)/CMSIS/Lib/GCC/
# Create a map file
LFLAGS += -Wl,-Map,"$@.map"
# Also tell the linker core architecture
LFLAGS += -mcpu=cortex-m4 -mthumb
# Inform about debug info
LFLAGS += -g3 -gdwarf-2

# startup files
LIBSRCS += startup_XMC$(XMC_SERIES).c system_XMC$(XMC_SERIES).c
# minimal syscalls definition for gcc
LIBSRCS += syscalls.c


# convert srcs to respective build targets
OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))
LIBOBJS = $(patsubst %.c,$(LIB_BUILDDIR)/%.o,$(LIBSRCS))

.PHONY: default program debug clean

PARENT_DIR := $(notdir $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../))
NOT_PACKING_STRING_1 := "NOTE: NOT PACKING THE FOLLOWING FILES: "
NOT_PACKING_STRING_2 := "(if you want to submit them, rename to 'Part[A-Z]' and make sure your *.elf, token, or exploit is present)"

default: $(BUILDDIR)/main.elf $(BUILDDIR)/main.lst

program: $(BUILDDIR)/main.hex $(BUILDDIR)/JLinkCommands
	JLinkExe -Device XMC$(XMC_SERIES) -If SWD -Speed 1000 -CommanderScript $(BUILDDIR)/JLinkCommands

debug: $(BUILDDIR)/main.elf $(BUILDDIR)/GDBCommands GDBStudentCommands
	echo "##### Debug session started at $(date) #####" >JLinkLog
	JLinkGDBServer -device XMC$(XMC_SERIES) -endian little -if SWD -speed 1000 -halt -logtofile -log JLinkLog -silent -vd &
	sleep 1
	sh -ci "$(DB) -command=$(BUILDDIR)/GDBCommands -command=GDBStudentCommands -tui"
	killall JLinkGDBServer

$(BUILDDIR)/%.bin: $(BUILDDIR)/%.elf
	$(OC) -O binary $< $@

$(BUILDDIR)/%.hex: $(BUILDDIR)/%.elf
	$(OC) -O ihex $< $@

$(BUILDDIR)/%.lst: $(BUILDDIR)/%.elf
	$(OD) -h -S $< > $@

$(BUILDDIR)/%.elf: $(OBJS) $(LIBOBJS) $(BUILDDIR)/$(LD_NAME).ld
	$(LD) $(LFLAGS) -o $@ $(OBJS) $(LIBOBJS) $(LIBLNK)
	$(SZ) $@

$(BUILDDIR)/%.o: %.c $(HDRS) | $(BUILDDIR)
	echo "building $@"
	$(CC) $(CFLAGS) -o $@ $<

$(LIB_BUILDDIR)/%.o: %.c | $(LIB_BUILDDIR)
	echo "building $@"
	$(CC) $(CFLAGS) -o $@ $<

# startup
$(LIB_BUILDDIR)/startup_XMC$(XMC_SERIES).o: $(XMC_LIBDIR)/CMSIS/Infineon/XMC$(XMC_SERIES)_series/Source/GCC/startup_XMC$(XMC_SERIES).S | $(LIB_BUILDDIR)
	$(CC) $(SFLAGS) -o $@ $<

# When copying the linker description file, see if there is one created by the student and only if not copy the one from the library
$(BUILDDIR)/$(LD_NAME).ld: $(XMC_LIBDIR)/CMSIS/Infineon/XMC$(XMC_SERIES)_series/Source/GCC/XMC$(XMC_SERIES)x$(XMC_SIZE).ld | $(BUILDDIR)
	if [ -f "$(LD_NAME).ld" ] ; then cp -pu "$(LD_NAME).ld" $@ ; else cp -n --preserve=timestamps $< $@ ; fi

$(BUILDDIR):
	mkdir -p $@

$(LIB_BUILDDIR):
	mkdir -p $@

# Write a script file for JLinkExe
$(BUILDDIR)/JLinkCommands: | $(BUILDDIR)
	echo "h;loadfile ${BUILDDIR}/main.hex;r;g;q;" | sed 's/;/\n/g' > $(BUILDDIR)/JLinkCommands

# Write a script file for GDB
$(BUILDDIR)/GDBCommands: | $(BUILDDIR)
	echo "file ${BUILDDIR}/main.elf;target remote localhost:2331;monitor reset;load;break main;" | sed 's/;/\n/g' > $(BUILDDIR)/GDBCommands

GDBStudentCommands:
	touch GDBStudentCommands

deliverable: $(BUILDDIR)/ZIPlist $(BUILDDIR)/noZIPlist
	@echo "packing deliverable"
	@if [ -s $(BUILDDIR)/noZIPlist ] ; then echo $(NOT_PACKING_STRING_1) && cat $(BUILDDIR)/noZIPlist && echo $(NOT_PACKING_STRING_2) ; fi
	@echo ""
	@sh -c "cd ..; cat $(abspath $(BUILDDIR)/ZIPlist) | zip $(PARENT_DIR).zip -r -@ -x */JLinkLog */noZIPlist"

$(BUILDDIR)/ZIPlist: $(BUILDDIR)
	@find ../ -regextype posix-extended -regex '\.{1,2}/Part[A-Z]/(token|exploit|.*.elf)' -printf '%h\0' | cut -z -d '/' -f 2 | sort -z -u | tr '\0' '\n' > $@

# Grep unfortunately exits 1 if nothing matched, which in this case is good, so fix the return value to only report values > 1, i.e. actual errors
$(BUILDDIR)/noZIPlist: $(BUILDDIR)/ZIPlist
	@ls ../ | grep -xvf $(BUILDDIR)/ZIPlist > $@ || exit `expr \( "$$?" \> 1 \) \* "$$?"`

clean:
	rm -rf $(BUILDDIR)

clean_all:
	rm -rf $(BUILDDIR) $(LIB_BUILDDIR)
