
/*--------------------------------------------------------------------*/
/*--- Nulgrind: The minimal Valgrind tool.               cs_main.c ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Nulgrind, the minimal Valgrind tool,
   which does no instrumentation or analysis.

   Copyright (C) 2002-2017 Nicholas Nethercote
      njn@valgrind.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.
*/

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_libcassert.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_debuginfo.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_options.h"
#include "pub_tool_machine.h"     // VG_(fnptr_to_fnentry)
#include <string.h>
#include <stdlib.h>

#define MAX_BREAKPOINTS 1024
#define MAX_INDICES 1024

static const HChar* outputname = "./memory_trace.bin";

static unsigned long breakpoint_begin[MAX_BREAKPOINTS];
static unsigned long breakpoint_begin_count = 0;
static long begin_bp = -1;

static unsigned long breakpoint_end[MAX_BREAKPOINTS];
static unsigned long breakpoint_end_count = 0;
static long end_bp = -1;

static unsigned long max_traces = -1;

static unsigned long indices[MAX_INDICES];
static const HChar* indices_str = NULL;
static unsigned long indices_count = 0; 

static unsigned int enabled = 0;
static unsigned int num_traces = 0;
static unsigned int cindice = 0;

typedef
   IRExpr 
   IRAtom;

#define MAX_DSIZE    512

typedef 
   enum { Event_Ir, Event_Dr, Event_Dw, Event_Dm }
   EventKind;

typedef
   struct {
      EventKind  ekind;
      IRAtom*    addr;
      Int        size;
      IRAtom*    guard; /* :: Ity_I1, or NULL=="always True" */
   }
   Event;

#define N_EVENTS 4

static Event events[N_EVENTS];
static Int   events_used = 0;

unsigned int trace_enabled(Addr addr);
unsigned int in_list(Addr addr, unsigned long* bps, unsigned long count);

static Bool cs_process_cmd_line_option(const HChar* arg)
{
   if VG_STR_CLO(arg, "-output", outputname) {}
   else if VG_BHEX_CLO(arg, "-begin", begin_bp, 0, 0xfffffffffffffffe) {
       if (breakpoint_begin_count==MAX_BREAKPOINTS) {
           VG_(tool_panic)("Too many -begin breakpoints\n");
       }
       breakpoint_begin[breakpoint_begin_count] = begin_bp;
       breakpoint_begin_count++;
       begin_bp = -1;
   }
   else if VG_BHEX_CLO(arg, "-end", end_bp, 0, 0xfffffffffffffffe) {
       if (breakpoint_end_count==MAX_BREAKPOINTS) {
           VG_(tool_panic)("Too many -end breakpoints\n");
       }
       breakpoint_end[breakpoint_end_count] = end_bp;
       breakpoint_end_count++;
       end_bp = -1;
   }
   else if VG_INT_CLO(arg, "-max-traces", max_traces) {
        if (max_traces < 0) max_traces = -1;
   }
   else if VG_STR_CLO(arg, "-indices", indices_str) {
        HChar* wcmd;
        HChar *ssaveptr; 
        HChar *the_end; 
        wcmd = VG_(strtok_r) (indices_str, ",", &ssaveptr);
        while (wcmd != NULL) {
            indices[indices_count] = VG_(strtoll10) (wcmd, &the_end); 
            indices_count++;
            wcmd = VG_(strtok_r) (NULL, ",", &ssaveptr);
        } 
   } else return False;
   
   tl_assert(outputname);
   tl_assert(outputname[0]);
   return True;
}

static void cs_print_usage(void)
{  
   VG_(printf)(
"    -output name             Output file name [./memory_trace.bin]\n"
"    -begin address           Begin address of the region of interest in hex format (can be specified multiple times)\n"
"    -end address             End address of the region of interest in hex format (can be specified multiple times)\n"
"    -max-traces num          Stop tracing after num traces have been extracted\n"
"    -indices num1,num2,num3  Comma separated list of the invocations to be executed\n"
   );
}

static void cs_print_debug_usage(void)
{  
   VG_(printf)(
"    (none)\n"
   );
}

static void cs_post_clo_init(void)
{
   unsigned int i;
   VG_(umsg)("Chopstix memory trace configuration:\n");
   VG_(umsg)("- Output: %s\n", outputname);
   VG_(umsg)("- Begin address: \n");
   for (i=0; i<breakpoint_begin_count; i++){
       VG_(umsg)("  - 0x%016lX\n", breakpoint_begin[i]);
   }
   VG_(umsg)("- End address: \n");
   for (i=0; i<breakpoint_end_count; i++){
       VG_(umsg)("  - 0x%016lX\n", breakpoint_end[i]);
   }
   VG_(umsg)("- Maximum traces: %lu\n", max_traces);
   VG_(umsg)("- Indices: \n");
   for (i=0; i<indices_count; i++){
       VG_(umsg)("  - %lu\n", indices[i]);
   }
}

unsigned int in_list(Addr addr, unsigned long* bps, unsigned long count)
{
    for(unsigned int i = 0; i < count; i++){
        if(addr == bps[i]) return 1;
    }
    return 0;
}

unsigned int trace_enabled(Addr addr) {

    unsigned int is_entry = in_list(addr, breakpoint_begin, breakpoint_begin_count);
    unsigned int is_exit = in_list(addr, breakpoint_end, breakpoint_end_count);

    if ((is_entry == 0) && (is_exit == 0)) return enabled;
    else if ((is_entry == 1) && (is_exit == 1)) {

        if(enabled && max_traces != -1)
        {
            if (num_traces >= max_traces) { VG_(exit)(0); }
        }

        if (num_traces >= indices_count && indices_count > 0) { VG_(exit)(0); }

        if(indices_count == 0) enabled = 1;
        else if (in_list(cindice, indices, indices_count)) enabled = 1;
        else enabled = 0;

        if (enabled) {
            VG_(printf)("# Begin trace index %d\n", cindice);
            num_traces++;
        }

        cindice++;

    }
    else if ((is_entry == 1) && (is_exit == 0)) {
        tl_assert(enabled == 0);
        
        if(indices_count == 0) enabled = 1;
        else if (in_list(cindice, indices, indices_count)) enabled = 1;

        if (enabled) {
            VG_(printf)("# Begin trace index %d\n", cindice);
            num_traces++;
        }
        cindice++;
    }
    else if ((is_entry == 0) && (is_exit == 1)) {
        if(enabled && max_traces != -1)
        {
            if (num_traces >= max_traces) { VG_(exit)(0); }
        }

        if (num_traces >= indices_count && indices_count > 0) { VG_(exit)(0); }

        enabled = 0;
    }
    
    return enabled;
}

static VG_REGPARM(2) void trace_instr(Addr addr, SizeT size)
{
   if (trace_enabled(addr)) {
       VG_(printf)("I R 0x%016lX %lu\n", addr, size);
   }
}

static VG_REGPARM(2) void trace_load(Addr addr, SizeT size)
{
   if (trace_enabled(addr)) {
       VG_(printf)("D R 0x%016lx %lu\n", addr, size);
   }
}

static VG_REGPARM(2) void trace_store(Addr addr, SizeT size)
{
   if (trace_enabled(addr)) {
      VG_(printf)("D W 0x%016lx %lu\n", addr, size);
   }
}

static VG_REGPARM(2) void trace_modify(Addr addr, SizeT size)
{
   if (trace_enabled(addr)) {
      VG_(printf)("D W 0x%016lx %lu\n", addr, size);
   }
}

static void flushEvents(IRSB* sb)
{
   Int        i;
   const HChar* helperName;
   void*      helperAddr;
   IRExpr**   argv;
   IRDirty*   di;
   Event*     ev;

   for (i = 0; i < events_used; i++) {

      ev = &events[i];
      
      // Decide on helper fn to call and args to pass it.
      switch (ev->ekind) {
         case Event_Ir: helperName = "trace_instr";
                        helperAddr =  trace_instr;  break;

         case Event_Dr: helperName = "trace_load";
                        helperAddr =  trace_load;   break;

         case Event_Dw: helperName = "trace_store";
                        helperAddr =  trace_store;  break;

         case Event_Dm: helperName = "trace_modify";
                        helperAddr =  trace_modify; break;
         default:
            tl_assert(0);
      }

      // Add the helper.
      argv = mkIRExprVec_2( ev->addr, mkIRExpr_HWord( ev->size ) );
      di   = unsafeIRDirty_0_N( /*regparms*/2, 
                                helperName, VG_(fnptr_to_fnentry)( helperAddr ),
                                argv );
      if (ev->guard) {
         di->guard = ev->guard;
      }
      addStmtToIRSB( sb, IRStmt_Dirty(di) );
   }

   events_used = 0;
}

// WARNING:  If you aren't interested in instruction reads, you can omit the
// code that adds calls to trace_instr() in flushEvents().  However, you
// must still call this function, addEvent_Ir() -- it is necessary to add
// the Ir events to the events list so that merging of paired load/store
// events into modify events works correctly.
static void addEvent_Ir ( IRSB* sb, IRAtom* iaddr, UInt isize )
{
   Event* evt;
   tl_assert( (VG_MIN_INSTR_SZB <= isize && isize <= VG_MAX_INSTR_SZB)
            || VG_CLREQ_SZB == isize );
   if (events_used == N_EVENTS)
      flushEvents(sb);
   tl_assert(events_used >= 0 && events_used < N_EVENTS);
   evt = &events[events_used];
   evt->ekind = Event_Ir;
   evt->addr  = iaddr;
   evt->size  = isize;
   evt->guard = NULL;
   events_used++;
}

/* Add a guarded read event. */
static
void addEvent_Dr_guarded ( IRSB* sb, IRAtom* daddr, Int dsize, IRAtom* guard )
{
   Event* evt;
   tl_assert(isIRAtom(daddr));
   tl_assert(dsize >= 1 && dsize <= MAX_DSIZE);
   if (events_used == N_EVENTS)
      flushEvents(sb);
   tl_assert(events_used >= 0 && events_used < N_EVENTS);
   evt = &events[events_used];
   evt->ekind = Event_Dr;
   evt->addr  = daddr;
   evt->size  = dsize;
   evt->guard = guard;
   events_used++;
}

/* Add an ordinary read event, by adding a guarded read event with an
   always-true guard. */
static
void addEvent_Dr ( IRSB* sb, IRAtom* daddr, Int dsize )
{
   addEvent_Dr_guarded(sb, daddr, dsize, NULL);
}

/* Add a guarded write event. */
static
void addEvent_Dw_guarded ( IRSB* sb, IRAtom* daddr, Int dsize, IRAtom* guard )
{
   Event* evt;
   tl_assert(isIRAtom(daddr));
   tl_assert(dsize >= 1 && dsize <= MAX_DSIZE);
   if (events_used == N_EVENTS)
      flushEvents(sb);
   tl_assert(events_used >= 0 && events_used < N_EVENTS);
   evt = &events[events_used];
   evt->ekind = Event_Dw;
   evt->addr  = daddr;
   evt->size  = dsize;
   evt->guard = guard;
   events_used++;
}

/* Add an ordinary write event.  Try to merge it with an immediately
   preceding ordinary read event of the same size to the same
   address. */
static
void addEvent_Dw ( IRSB* sb, IRAtom* daddr, Int dsize )
{
   Event* lastEvt;
   Event* evt;
   tl_assert(isIRAtom(daddr));
   tl_assert(dsize >= 1 && dsize <= MAX_DSIZE);

   // Is it possible to merge this write with the preceding read?
   lastEvt = &events[events_used-1];
   if (events_used > 0
       && lastEvt->ekind == Event_Dr
       && lastEvt->size  == dsize
       && lastEvt->guard == NULL
       && eqIRAtom(lastEvt->addr, daddr))
   {
      lastEvt->ekind = Event_Dm;
      return;
   }

   // No.  Add as normal.
   if (events_used == N_EVENTS)
      flushEvents(sb);
   tl_assert(events_used >= 0 && events_used < N_EVENTS);
   evt = &events[events_used];
   evt->ekind = Event_Dw;
   evt->size  = dsize;
   evt->addr  = daddr;
   evt->guard = NULL;
   events_used++;
}

static
IRSB* cs_instrument ( VgCallbackClosure* closure,
                      IRSB* sbIn,
                      const VexGuestLayout* layout, 
                      const VexGuestExtents* vge,
                      const VexArchInfo* archinfo_host,
                      IRType gWordTy, IRType hWordTy )
{

   IRDirty*   di;
   Int        i;
   IRSB*      sbOut;
   IRTypeEnv* tyenv = sbIn->tyenv;
   events_used = 0;

   if (gWordTy != hWordTy) {
      /* We don't currently support this case. */
      VG_(tool_panic)("host/guest word size mismatch");
   }

   /* Set up SB */
   sbOut = deepCopyIRSBExceptStmts(sbIn);

   // Copy verbatim any IR preamble preceding the first IMark
   i = 0;
   while (i < sbIn->stmts_used && sbIn->stmts[i]->tag != Ist_IMark) {
      addStmtToIRSB( sbOut, sbIn->stmts[i] );
      i++;
   }

   for (/*use current i*/; i < sbIn->stmts_used; i++) {
      IRStmt* st = sbIn->stmts[i];
      if (!st || st->tag == Ist_NoOp) continue;

      switch (st->tag) {
         case Ist_NoOp:
         case Ist_AbiHint:
         case Ist_Put:
         case Ist_PutI:
         case Ist_MBE: {
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_IMark: {
            addEvent_Ir( sbOut, mkIRExpr_HWord( (HWord)st->Ist.IMark.addr ), st->Ist.IMark.len );
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_WrTmp: {
            IRExpr* data = st->Ist.WrTmp.data;
            if (data->tag == Iex_Load) {
               addEvent_Dr( sbOut, data->Iex.Load.addr,
                            sizeofIRType(data->Iex.Load.ty) );
            }
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_Store: {
            IRExpr* data = st->Ist.Store.data;
            IRType  type = typeOfIRExpr(tyenv, data);
            tl_assert(type != Ity_INVALID);
            addEvent_Dw( sbOut, st->Ist.Store.addr, sizeofIRType(type) );
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_StoreG: { 
            IRStoreG* sg   = st->Ist.StoreG.details;
            IRExpr*   data = sg->data;
            IRType    type = typeOfIRExpr(tyenv, data);
            tl_assert(type != Ity_INVALID);
            addEvent_Dw_guarded( sbOut, sg->addr,
                                 sizeofIRType(type), sg->guard );
            addStmtToIRSB( sbOut, st );
            break; }
         
         case Ist_LoadG: {
            IRLoadG* lg       = st->Ist.LoadG.details;
            IRType   type     = Ity_INVALID; /* loaded type */
            IRType   typeWide = Ity_INVALID; /* after implicit widening */
            typeOfIRLoadGOp(lg->cvt, &typeWide, &type);
            tl_assert(type != Ity_INVALID);
            addEvent_Dr_guarded( sbOut, lg->addr,
                                 sizeofIRType(type), lg->guard );
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_Dirty: {
            Int      dsize;
            IRDirty* d = st->Ist.Dirty.details;
            if (d->mFx != Ifx_None) {
               // This dirty helper accesses memory.  Collect the details.
               tl_assert(d->mAddr != NULL);
               tl_assert(d->mSize != 0);
               dsize = d->mSize;
               if (d->mFx == Ifx_Read || d->mFx == Ifx_Modify)
                  addEvent_Dr( sbOut, d->mAddr, dsize );
               if (d->mFx == Ifx_Write || d->mFx == Ifx_Modify)
                  addEvent_Dw( sbOut, d->mAddr, dsize );
            } else {
               tl_assert(d->mAddr == NULL);
               tl_assert(d->mSize == 0);
            }
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_CAS: {
            /* We treat it as a read and a write of the location.  I
               think that is the same behaviour as it was before IRCAS
               was introduced, since prior to that point, the Vex
               front ends would translate a lock-prefixed instruction
               into a (normal) read followed by a (normal) write. */
            Int    dataSize;
            IRType dataTy;
            IRCAS* cas = st->Ist.CAS.details;
            tl_assert(cas->addr != NULL);
            tl_assert(cas->dataLo != NULL);
            dataTy   = typeOfIRExpr(tyenv, cas->dataLo);
            dataSize = sizeofIRType(dataTy);
            if (cas->dataHi != NULL)
               dataSize *= 2; /* since it's a doubleword-CAS */
            addEvent_Dr( sbOut, cas->addr, dataSize );
            addEvent_Dw( sbOut, cas->addr, dataSize );
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_LLSC: {
            IRType dataTy;
            if (st->Ist.LLSC.storedata == NULL) {
               /* LL */
               dataTy = typeOfIRTemp(tyenv, st->Ist.LLSC.result);
               addEvent_Dr( sbOut, st->Ist.LLSC.addr,
                                   sizeofIRType(dataTy) );
               /* flush events before LL, helps SC to succeed */
               flushEvents(sbOut);
            } else {
               /* SC */
               dataTy = typeOfIRExpr(tyenv, st->Ist.LLSC.storedata);
               addEvent_Dw( sbOut, st->Ist.LLSC.addr,
                                   sizeofIRType(dataTy) );
            }
            addStmtToIRSB( sbOut, st );
            break; }

         case Ist_Exit: {
            flushEvents(sbOut);
            addStmtToIRSB( sbOut, st );      // Original statement
            break; }
 
         default: {
            ppIRStmt(st);
            tl_assert(0); }
      }
   }

   /* At the end of the sbIn.  Flush outstandings. */
   flushEvents(sbOut);
   return sbOut;
}

static void cs_fini(Int exitcode)
{
}

static void cs_pre_clo_init(void)
{
   VG_(details_name)            ("Chopstix");
   VG_(details_version)         (NULL);
   VG_(details_description)     ("Chopstix memory tracing tool");
   VG_(details_copyright_author)(
      "Copyright (C) 2022 IBM Corporation");
   VG_(details_bug_reports_to)  (VG_BUGS_TO);
   VG_(details_avg_translation_sizeB) ( 275 );

   VG_(basic_tool_funcs)        (cs_post_clo_init,
                                 cs_instrument,
                                 cs_fini);

   VG_(needs_command_line_options)(cs_process_cmd_line_option,
                                   cs_print_usage,
                                   cs_print_debug_usage);

   /* No needs, no core events to track */
}

VG_DETERMINE_INTERFACE_VERSION(cs_pre_clo_init)

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
