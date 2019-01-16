# Name of your project
LD_NAME = Project

# Add lists of space separated source files
# Own sources, e.g. main.c
SRCS = main.c KeyboardHID.c Descriptors.c
# Header files for configuration. Adding the header file will  make make compile on changes.
HDRS =
# Library sources, e.g. xmc_gpio.c
LIBSRCS = xmc4_scu.c xmc_gpio.c HIDClassDevice.c memcpy.c Endpoint_XMC4000.c EndpointStream_XMC4000.c USBController_XMC4000.c xmc_usbd.c ConfigDescriptors.c HostStandardReq.c USBTask.c Events.c
# Precompiled libraries, e.g. -lm for math functions
LIBLNK =

# Change this line if the path to your XMC-Library differs, it will be overwritten before grading to
# match the system the grading is performed on.
XMC_LIBDIR = /opt/XMClib/XMC_Peripheral_Library_v2.1.16

# Language dialect
SCFLAGS  = -std=gnu99
# Optimization level, remember that enabling optimization will stirr up the assembly code and thus
# debugging is more difficult
SCFLAGS += -O0
# If you like, you can enable even more warnings, e.g. -Wextra, but for grading -Wall will be used
# You can also add any other compiler flag here if either necessary or you find it appropriate
SCFLAGS +=

