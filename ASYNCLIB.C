// Power C include file.  I don't like doing it this way.  If anyone is more
// familiar with Power C and would like to redo this, be my guest.  Please
// send anything new to me though.
//
// This basically includes all of the .C files so we can compile them into
// one big .MIX file.  MERGE doesn't work very well from a project file
// for some reason, otherwise I would have used it.  Such is life eh?

#ifndef POWERC
This file should ONLY be compiled when compiling for Power C.
#endif

#include "async.c"
#include "setuart.c"
#include "isr.c"
#include "asyio.c"
#include "detect.c"
#include "line.c"
#include "fifo.c"
#include "hshake.c"
#include "pc_comp.c"
