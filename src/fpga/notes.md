# Notes

Ok so everything sucks. You can't even get it to build with the new changes.

If you truly want it to be modular,
You will need to fix up the init functions in device.c and some other files. You can esenttial rename to fpga_ and then within fpga.c, do the work to handle checks and init per device.
