#
# AX_ULX3S
#
# This software is Copyright (c) 2016 Denis Burykin
# [denis_burykin yahoo com], [denis-burykin2014 yandex ru]
# and it is hereby released to the general public under the following terms:
# Redistribution and use in source and binary forms, with or without
# modification, are permitted.
#
# If configured to support ULX3S or ZTEX USB-FPGA module with --enable-fpga=yes|ztex|ulx3s:
# - check for headers and libraries
# - add Makefiles
# - set ULX3S_SUBDIRS, ULX3S_LIBS
#
AC_DEFUN_ONCE([AX_FPGA], [

AC_ARG_ENABLE([fpga],
  [AC_HELP_STRING([--enable-fpga],[Support FPGAs])],
  [fpga=$enableval], [fpga=ztex])

ULX3S_SUBDIRS=""
ULX3S_LIBS=""

if test "x$fpga" = xulx3s; then

AC_CHECK_HEADER([libusb-1.0/libusb.h],
  [AC_CHECK_LIB([usb-1.0], [libusb_init],
    [],
    [AC_MSG_FAILURE(FPGA module requires libusb-1.0.)]
  )],
  [AC_MSG_FAILURE(FPGA module requires libusb-1.0.)]
)

AC_CONFIG_FILES([fpga/ulx3s/Makefile fpga/ulx3s/pkt_comm/Makefile])

ULX3S_SUBDIRS="fpga/ulx3s"
ULX3S_LIBS="fpga/ulx3s/device.o fpga/ulx3s/device_format.o fpga/ulx3s/inouttraffic.o fpga/ulx3s/jtr_device.o fpga/ulx3s/jtr_mask.o fpga/ulx3s/task.o fpga/ulx3s/ulx3s.o fpga/ulx3s/ulx3s_scan.o fpga/ulx3s/ulx3s_sn.o fpga/ulx3s/pkt_comm/*.o -lusb-1.0"

fi

if test "x$ztex" = xyes; then

AC_CHECK_HEADER([libusb-1.0/libusb.h],
  [AC_CHECK_LIB([usb-1.0], [libusb_init],
    [],
    [AC_MSG_FAILURE(ZTEX USB-FPGA module requires libusb-1.0.)]
  )],
  [AC_MSG_FAILURE(ZTEX USB-FPGA module requires libusb-1.0.)]
)

AC_CONFIG_FILES([fpga/ztex/Makefile fpga/ztex/pkt_comm/Makefile])

ZTEX_SUBDIRS="fpga/ztex"
ZTEX_LIBS="fpga/ztex/device.o fpga/ztex/device_format.o fpga/ztex/inouttraffic.o fpga/ztex/jtr_device.o fpga/ztex/jtr_mask.o fpga/ztex/task.o fpga/ztex/ztex.o fpga/ztex/ztex_scan.o fpga/ztex/ztex_sn.o fpga/ztex/pkt_comm/*.o -lusb-1.0"

fi

AC_SUBST([ZTEX_SUBDIRS])
AC_SUBST([ZTEX_LIBS])

AC_SUBST([ULX3S_SUBDIRS])
AC_SUBST([ULX3S_LIBS])
])dnl
