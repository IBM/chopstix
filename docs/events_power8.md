# Events and groups supported on POWER8 architecture

On Linux POWER8, the number of CPU groups supported is 267. The following tables list the supported event groups listed by group ID number.

### Group 0
Event | Description
--- | ---
PM_CYC | Cycles
PM_RUN_CYC | Run cycles
PM_INST_DISP | PPC Dispatched
PM_INST_CMPL | Number of PowerPC® Instructions that completed. PPC Instructions Finished (completed.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 1
Event | Description
--- | ---
PM_RUN_SPURR | Run SPURR
PM_RUN_CYC | Run cycles
PM_CYC | Cycles
PM_RUN_PURR | Run_PURR
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 2
Event | Description
--- | ---
PM_IOPS_CMPL | Internal Operations completed
PM_CYC | Cycles
PM_IOPS_DISP | Internal Operations dispatched
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 3
Event | Description
--- | ---
PM_RUN_CYC_ST_MODE | Cycles run latch is set and core is in ST mode
PM_RUN_CYC_SMT2_SHRD_MODE | Cycles this threads run latch is set and the core is in SMT2 shared mode
PM_RUN_CYC_SMT2_MODE | Cycles run latch is set and core is in SMT2 mode
PM_RUN_CYC_SMT8_MODE | Cycles run latch is set and core is in SMT8 mode
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 4
Event | Description
--- | ---
PM_RUN_CYC_SMT2_SPLIT_MODE | Cycles run latch is set and core is in SMT2 split mode
PM_RUN_CYC_SMT4_MODE | Cycles this threads run latch is set and the core is in SMT4 mode
PM_RUN_CYC_SMT2_MODE | Cycles run latch is set and core is in SMT2 mode
PM_RUN_CYC_SMT8_MODE | Cycles run latch is set and core is in SMT8 mode
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 5
Event | Description
--- | ---
PM_THRD_GRP_CMPL_BOTH_CYC | Cycles group completed on both completion slots by any thread
PM_THRD_ALL_RUN_CYC | All Threads in run_cycles (was both threads in run_cycles)
PM_THRD_CONC_RUN_INST | PPC Instructions Finished when both threads in run_cycles
PM_THRD_PRIO_0_1_CYC | Cycles thread running at priority level 0 or 1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 6
Event | Description
--- | ---
PM_THRD_PRIO_0_1_CYC | Cycles thread running at priority level 0 or 1
PM_THRD_PRIO_2_3_CYC | Cycles thread running at priority level 2 or 3
PM_THRD_PRIO_4_5_CYC | Cycles thread running at priority level 4 or 5
PM_THRD_PRIO_6_7_CYC | Cycles thread running at priority level 6 or 7
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 7
Event | Description
--- | ---
PM_ANY_THRD_RUN_CYC | One of threads in run_cycles
PM_THRD_REBAL_CYC | Cycles rebalance was active
PM_NEST_REF_CLK | Multiply by 4 to obtain the number of PB cycles
PM_RUN_INST_CMPL | Run instructions
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 8
Event | Description
--- | ---
PM_BR_PRED_BR0 | Conditional Branch Completed on BR0 (1st branch in group) in which the HW predicted the Direction or Target
PM_BR_PRED_BR1 | Conditional Branch Completed on BR1 (2nd branch in group) in which the HW predicted the Direction or Target. Note: BR1 can only be used in Single Thread Mode. In all of the SMT modes, only one branch can complete, thus BR1 is unused.
PM_BR_UNCOND_BR0 | Unconditional Branch Completed on BR0. HW branch prediction was not used for this branch. This can be an I form branch, a B form branch with BO field set to branch always, or a B form branch which was converted to a Resolve.
PM_BR_UNCOND_BR1 | Unconditional Branch Completed on BR1. HW branch prediction was not used for this branch. This can be an I form branch, a B form branch with BO field set to branch always, or a B form branch which was converted to a Resolve.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 9
Event | Description
--- | ---
PM_BR_PRED_CCACHE_BR0 | Conditional Branch Completed on BR0 that used the Count Cache for Target Prediction
PM_BR_PRED_CCACHE_BR1 | Conditional Branch Completed on BR1 that used the Count Cache for Target Prediction
PM_BR_PRED_LSTACK_BR0 | Conditional Branch Completed on BR0 that used the Link Stack for Target Prediction
PM_BR_PRED_LSTACK_BR1 | Conditional Branch Completed on BR1 that used the Link Stack for Target Prediction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 10
Event | Description
--- | ---
PM_BR_PRED_CR_BR0 | Conditional Branch Completed on BR0 that had its direction predicted. I form branches do not set this event. In addition, B form branches which do not use the BHT do not set this event these are branches with BO field set to 'always taken' and branches
PM_BR_PRED_CR_BR1 | Conditional Branch Completed on BR1 that had its direction predicted. I form branches do not set this event. In addition, B form branches which do not use the BHT do not set this event these are branches with BO field set to 'always taken' and branches
PM_BR_PRED_TA_BR0 | Conditional Branch Completed on BR0 that had its target address predicted. Only XL form branches set this event.
PM_BR_PRED_TA_BR1 | Conditional Branch Completed on BR1 that had its target address predicted. Only XL form branches set this event.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 11
Event | Description
--- | ---
PM_BRU_FIN | Branch Instruction Finished
PM_BR_TAKEN_CMPL | New event for Branch Taken
PM_BR_PRED_BR_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred(0) OR if_pc_br0_br_pred(1).
PM_BR_CMPL | Branch Instruction completed
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 12
Event | Description
--- | ---
PM_BR_BC_8_CONV | Pairable BC+8 branch that was converted to a Resolve Finished in the BRU pipeline.
PM_BR_BC_8 | Pairable BC+8 branch that has not been converted to a Resolve Finished in the BRU pipeline
PM_BR_UNCOND_BR0 | Unconditional Branch Completed on BR0. HW branch prediction was not used for this branch. This can be an I form branch, a B form branch with BO field set to branch always, or a B form branch which was converted to a Resolve.
PM_BR_2PATH | Two path branch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 13
Event | Description
--- | ---
PM_BR_MPRED_LSTACK | Conditional Branch Completed that was Mispredicted due to the Link Stack Target Prediction
PM_BR_MPRED_CCACHE | Conditional Branch Completed that was Mispredicted due to the Count Cache Target Prediction
PM_BR_MPRED_CR | Conditional Branch Completed that was Mispredicted due to the BHT Direction Prediction (taken/not taken).
PM_BR_MPRED_TA | Conditional Branch Completed that was Mispredicted due to the Target Address Prediction from the Count Cache or Link Stack. Only XL form branches that resolved Taken set this event.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 14
Event | Description
--- | ---
PM_FLUSH_DISP_SYNC | Dispatch flush: Sync
PM_FLUSH_BR_MPRED | Flush caused by branch mispredict
PM_FLUSH_DISP_SB | Dispatch Flush: Scoreboard
PM_FLUSH | Flush (any type)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 15
Event | Description
--- | ---
PM_FLUSH_DISP | Dispatch flush
PM_FLUSH_PARTIAL | Partial flush
PM_FLUSH_COMPLETION | Completion Flush
PM_BR_MPRED_CMPL | Number of Branch Mispredicts
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 16
Event | Description
--- | ---
PM_DTLB_MISS_16G | Data TLB Misses, page size 16G
PM_DTLB_MISS_4K | Data TLB Misses, page size 4k
PM_DTLB_MISS_64K | Data TLB Miss page size 64K
PM_DTLB_MISS_16M | Data TLB Miss page size 16M
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 17
Event | Description
--- | ---
PM_DERAT_MISS_4K | Data ERAT Miss (Data TLB Access) page size 4K
PM_DERAT_MISS_64K | Data ERAT Miss (Data TLB Access) page size 64K
PM_DERAT_MISS_16M | Data ERAT Miss (Data TLB Access) page size 16M
PM_DERAT_MISS_16G | Data ERAT Miss (Data TLB Access) page size 16G
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 18
Event | Description
--- | ---
PM_FLUSH_DISP_SYNC | Dispatch flush: Sync
PM_FLUSH_DISP_TLBIE | Dispatch Flush: TLBIE
PM_FLUSH_DISP_SB | Dispatch Flush: Scoreboard
PM_FLUSH | Flush (any type)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 19
Event | Description
--- | ---
PM_FLUSH_DISP | Dispatch flush
PM_CYC | Cycles
PM_FLUSH_COMPLETION | Completion Flush
PM_FLUSH | Flush (any type)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 20
Event | Description
--- | ---
PM_FXU_IDLE | FXU0 idle and FXU1 idle
PM_FXU_BUSY | FXU0 busy and FXU1 busy.
PM_FXU0_BUSY_FXU1_IDLE | FXU0 busy and FXU1 idle
PM_FXU1_BUSY_FXU0_IDLE | FXU0 idle and FXU1 busy.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 21
Event | Description
--- | ---
PM_FXU0_FIN | The fixed point unit Unit 0 finished an instruction. Instructions that finish may not necessary complete.
PM_RUN_CYC | Run cycles
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_FXU1_FIN | FXU1 finished
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 22
Event | Description
--- | ---
PM_CYC | Cycles
PM_FXU_BUSY | FXU0 busy and FXU1 busy.
PM_FXU0_BUSY_FXU1_IDLE | FXU0 busy and FXU1 idle
PM_FXU1_BUSY_FXU0_IDLE | FXU0 idle and FXU1 busy.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 23
Event | Description
--- | ---
PM_FXU_IDLE | FXU0 idle and FXU1 idle
PM_FXU_BUSY | FXU0 busy and FXU1 busy.
PM_CYC | Cycles
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 24
Event | Description
--- | ---
PM_DISP_CLB_HELD_BAL | Dispatch/CLB Hold: Balance
PM_DISP_CLB_HELD_RES | Dispatch/CLB Hold: Resource
PM_DISP_CLB_HELD_TLBIE | Dispatch Hold: Due to TLBIE
PM_DISP_CLB_HELD_SYNC | Dispatch/CLB Hold: Sync type instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 25
Event | Description
--- | ---
PM_DISP_CLB_HELD_SB | Dispatch/CLB Hold: Scoreboard
PM_DISP_HELD_IQ_FULL | Dispatch held due to Issue q full
PM_DISP_HELD_SRQ_FULL | Dispatch held due SRQ no room
PM_DISP_HELD_SYNC_HOLD | Dispatch held due to SYNC hold
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 26
Event | Description
--- | ---
PM_DISP_HELD_MAP_FULL | Dispatch for this thread was held because the Mappers were full
PM_INST_DISP | PPC Dispatched
PM_GRP_DISP | Group dispatch
PM_1PLUS_PPC_DISP | Cycles at least one Instr Dispatched
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 27
Event | Description
--- | ---
PM_1PLUS_PPC_CMPL | 1 or more ppc instructions finished
PM_NTCG_ALL_FIN | Cycles after all instructions have finished to group completed
PM_GRP_CMPL | Group completed
PM_CYC | Cycles
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 28
Event | Description
--- | ---
PM_CMPLU_STALL | Completion stall
PM_CMPLU_STALL_FXU | Completion stall due to FXU
PM_CMPLU_STALL_FLUSH | Completion stall due to flush by own thread
PM_SHL_ST_DISABLE | Store Hit Load Table Read Hit with entry Disabled (entry was disabled due to the entry shown to not prevent the flush)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 29
Event | Description
--- | ---
PM_CMPLU_STALL_THRD | Completion Stalled due to thread conflict. Group ready to complete but it was another thread's turn
PM_CMPLU_STALL_BRU_CRU | Completion stall due to IFU
PM_CMPLU_STALL_COQ_FULL | Completion stall due to CO q full
PM_CMPLU_STALL_BRU | Completion stall due to a Branch Unit
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 30
Event | Description
--- | ---
PM_DATA_FROM_L2 | The processor's data cache was reloaded from local core's L2 due to a demand load
PM_CMPLU_STALL_DCACHE_MISS | Completion stall by Dcache miss
PM_CMPLU_STALL_HWSYNC | Completion stall due to hwsync
PM_CMPLU_STALL_DMISS_L2L3_CONFLICT | Completion stall due to cache miss that resolves in the L2 or L3 with a conflict
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 31
Event | Description
--- | ---
PM_DATA_FROM_L3_NO_CONFLICT | The processor's data cache was reloaded from local core's L3 without conflict due to a demand load
PM_CMPLU_STALL_DMISS_L21_L31 | Completion stall by Dcache miss which resolved on chip ( excluding local L2/L3)
PM_CMPLU_STALL_MEM_ECC_DELAY | Completion stall due to memory ECC delay
PM_CMPLU_STALL_DMISS_L3MISS | Completion stall due to cache miss resolving missed the L3
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 32
Event | Description
--- | ---
PM_DISP_HELD | Dispatch Held
PM_CMPLU_STALL_DMISS_L2L3 | Completion stall by Dcache miss which resolved in L2/L3
PM_CMPLU_STALL_OTHER_CMPL | Instructions core completed while this tread was stalled
PM_CMPLU_STALL_DMISS_LMEM | Completion stall due to cache miss that resolves in local memory
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 33
Event | Description
--- | ---
PM_FLOP | Floating Point Operation Finished
PM_CMPLU_STALL_DMISS_REMOTE | Completion stall by Dcache miss which resolved from remote chip (cache or memory)
PM_DISP_WT | Dispatched Starved
PM_CMPLU_STALL_ERAT_MISS | Completion stall due to LSU reject ERAT miss
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 34
Event | Description
--- | ---
PM_GCT_NOSLOT_CYC | Pipeline empty (No itags assigned , no GCT slots used)
PM_CMPLU_STALL_LSU | Completion stall by LSU instruction
PM_FXU0_BUSY_FXU1_IDLE | FXU0 busy and FXU1 idle
PM_CMPLU_STALL_FXLONG | Completion stall due to a long latency fixed point instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 35
Event | Description
--- | ---
PM_DATA_ALL_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for either demand loads or data prefetch
PM_CMPLU_STALL_NTCG_FLUSH | Completion stall due to ntcg flush
PM_L3_CO_MEPF | L3 castouts in Mepf state
PM_CMPLU_STALL_LOAD_FINISH | Completion stall due to a Load finish
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 36
Event | Description
--- | ---
PM_IC_DEMAND_CYC | Cycles when a demand ifetch was pending
PM_CMPLU_STALL_REJECT_LHS | Completion stall due to reject (load hit store)
PM_L3_SW_PREF | Data stream touch to L3
PM_CMPLU_STALL_REJECT | Completion stall due to LSU reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 37
Event | Description
--- | ---
PM_L1_DCACHE_RELOADED_ALL | L1 data cache reloaded for demand or prefetch
PM_CMPLU_STALL_SCALAR_LONG | Completion stall due to VSU scalar long latency instruction
PM_LSU_LMQ_SRQ_EMPTY_ALL_CYC | ALL threads lsu empty (lmq and srq empty)
PM_CMPLU_STALL_REJ_LMQ_FULL | Completion stall due to LSU reject LMQ full
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 38
Event | Description
--- | ---
PM_L2_TM_REQ_ABORT | TM abort
PM_CMPLU_STALL_STORE | Completion stall by stores this includes store agen finishes in pipe LS0/LS1 and store data finishes in LS2/LS3
PM_MRK_STALL_CMPLU_CYC | Marked Group completion Stall
PM_CMPLU_STALL_SCALAR | Completion stall due to VSU scalar instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 39
Event | Description
--- | ---
PM_L3_CO_MEPF | L3 castouts in Mepf state
PM_CMPLU_STALL_VECTOR | Completion stall due to VSU vector instruction
PM_MRK_ST_CMPL | Marked store completed and sent to nest
PM_CMPLU_STALL_ST_FWD | Completion stall due to store forward
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 40
Event | Description
--- | ---
PM_L3_LD_PREF | L3 Load Prefetches
PM_CMPLU_STALL_VSU | Completion stall due to VSU instruction
PM_ST_MISS_L1 | Store Missed L1
PM_CMPLU_STALL_VECTOR_LONG | Completion stall due to VSU vector long instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 41
Event | Description
--- | ---
PM_LSU0_REJECT | LSU0 reject
PM_GCT_NOSLOT_DISP_HELD_ISSQ | Gct empty for this thread due to dispatch hold on this thread due to Issue q full
PM_IFU_L2_TOUCH | L2 touch to update MRU on a line
PM_GCT_NOSLOT_BR_MPRED | Gct empty for this thread due to branch mispredict
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 42
Event | Description
--- | ---
PM_LSU_REJECT_LMQ_FULL | LSU reject due to LMQ full ( 4 per cycle)
PM_GCT_NOSLOT_DISP_HELD_OTHER | Gct empty for this thread due to dispatch hold on this thread due to sync
PM_LSU_FIN | LSU Finished an instruction (up to 2 per cycle)
PM_GCT_NOSLOT_BR_MPRED_ICMISS | Gct empty for this thread due to Icache Miss and branch mispredict
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 43
Event | Description
--- | ---
PM_DATA_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for a demand load
PM_GCT_NOSLOT_DISP_HELD_SRQ | Gct empty for this thread due to dispatch hold on this thread due to SRQ full
PM_FLUSH_LSU | Flush initiated by LSU
PM_GCT_NOSLOT_DISP_HELD_MAP | Gct empty for this thread due to dispatch hold on this thread due to Mapper full
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 44
Event | Description
--- | ---
PM_MRK_ST_CMPL | Marked store completed and sent to nest
PM_GCT_NOSLOT_IC_MISS | Gct empty for this thread due to Icache Miss
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_GCT_NOSLOT_IC_L3MISS | Gct empty for this thread due to icach L3 miss
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 45
Event | Description
--- | ---
PM_MEM_LOC_THRESH_IFU | Local Memory above threshold for IFU speculation control
PM_CMPLU_STALL_NO_NTF | Completion stall due to nop
PM_L1_DCACHE_RELOAD_VALID | DL1 reloaded due to Demand Load
PM_DATA_FROM_OFF_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 46
Event | Description
--- | ---
PM_CMPLU_STALL_LWSYNC | Completion stall due to isync/lwsync
PM_MEM_PREF | Memory prefetch for this lpar. Includes L4
PM_UP_PREF_L3 | Micropartition prefetch
PM_UP_PREF_POINTER | Micropartition pointer prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 47
Event | Description
--- | ---
PM_DC_PREF_STREAM_ALLOC | Stream marked valid. The stream could have been allocated through the hardware prefetch mechanism or through software. This is combined LS0 and LS1
PM_DC_PREF_STREAM_CONF | A demand load referenced a line in an active prefetch stream. The stream could have been allocated through the hardware prefetch mechanism or through software. Combine up + down
PM_DC_PREF_STREAM_STRIDED_CONF | A demand load referenced a line in an active strided prefetch stream. The stream could have been allocated through the hardware prefetch mechanism or through software.
PM_DC_PREF_STREAM_FUZZY_CONF | A demand load referenced a line in an active fuzzy prefetch stream. The stream could have been allocated through the hardware prefetch mechanism or through software. Fuzzy stream confirm (out of order effects, or pf cant keep up)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 48
Event | Description
--- | ---
PM_LD_CMPL | Count of Loads completed
PM_L3_ST_PREF | L3 store Prefetches
PM_L3_SW_PREF | Data stream touch to L3
PM_L3_PREF_ALL | Total HW L3 prefetches(Load+store)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 49
Event | Description
--- | ---
PM_DATA_FROM_L2 | The processor's data cache was reloaded from local core's L2 due to a demand load
PM_DATA_FROM_L2MISS | Demand LD L2 Miss (not L2 hit)
PM_DATA_FROM_L3MISS | Demand LD L3 Miss (not L2 hit and not L3 hit)
PM_DATA_FROM_L3 | The processor's data cache was reloaded from local core's L3 due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 50
Event | Description
--- | ---
PM_DATA_FROM_L2_NO_CONFLICT | The processor's data cache was reloaded from local core's L2 without conflict due to a demand load
PM_DATA_FROM_L2_MEPF | The processor's data cache was reloaded from local core's L2 hit without dispatch conflicts on Mepf state due to a demand load
PM_DATA_FROM_L2_DISP_CONFLICT_LDHITST | The processor's data cache was reloaded from local core's L2 with load hit store conflict due to a demand load
PM_DATA_FROM_L2_DISP_CONFLICT_OTHER | The processor's data cache was reloaded from local core's L2 with dispatch conflict due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 51
Event | Description
--- | ---
PM_DATA_FROM_L3_NO_CONFLICT | The processor's data cache was reloaded from local core's L3 without conflict due to a demand load
PM_DATA_FROM_L3_MEPF | The processor's data cache was reloaded from local core's L3 without dispatch conflicts hit on Mepf state due to a demand load
PM_DATA_FROM_L3_DISP_CONFLICT | The processor's data cache was reloaded from local core's L3 with dispatch conflict due to a demand load
PM_DATA_FROM_L3MISS_MOD | The processor's data cache was reloaded from a location other than the local core's L3 due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 52
Event | Description
--- | ---
PM_DATA_FROM_L31_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L3 on the same chip due to a demand load
PM_DATA_FROM_L31_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L3 on the same chip due to a demand load
PM_DATA_FROM_L31_ECO_SHR | The processor's data cache was reloaded with Shared (S) data from another core's ECO L3 on the same chip due to a demand load
PM_DATA_FROM_L31_ECO_MOD | The processor's data cache was reloaded with Modified (M) data from another core's ECO L3 on the same chip due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 53
Event | Description
--- | ---
PM_DATA_FROM_L2MISS_MOD | The processor's data cache was reloaded from a location other than the local core's L2 due to a demand load
PM_DATA_FROM_LMEM | The processor's data cache was reloaded from the local chip's Memory due to a demand load
PM_DATA_FROM_RMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group ( Remote) due to a demand load
PM_DATA_FROM_DMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group (Distant) due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 54
Event | Description
--- | ---
PM_DATA_FROM_ON_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on the same chip due to a demand load
PM_DATA_FROM_RL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a demand load
PM_DATA_FROM_L21_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L2 on the same chip due to a demand load
PM_DATA_FROM_L21_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L2 on the same chip due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 55
Event | Description
--- | ---
PM_DATA_FROM_LL4 | The processor's data cache was reloaded from the local chip's L4 cache due to a demand load
PM_DATA_FROM_RL4 | The processor's data cache was reloaded from another chip's L4 on the same Node or Group ( Remote) due to a demand load
PM_DATA_FROM_DL4 | The processor's data cache was reloaded from another chip's L4 on a different Node or Group (Distant) due to a demand load
PM_DATA_FROM_MEM | The processor's data cache was reloaded from a memory location including L4 from local remote or distant due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 56
Event | Description
--- | ---
PM_DATA_FROM_RL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a demand load
PM_DATA_FROM_MEMORY | The processor's data cache was reloaded from a memory location including L4 from local remote or distant due to a demand load
PM_DATA_FROM_DL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a demand load
PM_DATA_FROM_DL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 57
Event | Description
--- | ---
PM_DATA_ALL_FROM_L2 | The processor's data cache was reloaded from local core's L2 due to either demand loads or data prefetch
PM_FLOP_SUM_SCALAR | Flops summary scalar instructions
PM_FLOP_SUM_VEC | Flops summary vector instructions
PM_DATA_ALL_FROM_L3 | The processor's data cache was reloaded from local core's L3 due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 58
Event | Description
--- | ---
PM_DATA_ALL_FROM_L2_NO_CONFLICT | The processor's data cache was reloaded from local core's L2 without conflict due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L2_MEPF | The processor's data cache was reloaded from local core's L2 hit without dispatch conflicts on Mepf state due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L2_DISP_CONFLICT_LDHITST | The processor's data cache was reloaded from local core's L2 with load hit store conflict due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L2_DISP_CONFLICT_OTHER | The processor's data cache was reloaded from local core's L2 with dispatch conflict due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 59
Event | Description
--- | ---
PM_DATA_ALL_FROM_L3_NO_CONFLICT | The processor's data cache was reloaded from local core's L3 without conflict due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L3_MEPF | The processor's data cache was reloaded from local core's L3 without dispatch conflicts hit on Mepf state due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L3_DISP_CONFLICT | The processor's data cache was reloaded from local core's L3 with dispatch conflict due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L3MISS_MOD | The processor's data cache was reloaded from a location other than the local core's L3 due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 60
Event | Description
--- | ---
PM_DATA_ALL_FROM_L31_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L3 on the same chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L31_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L3 on the same chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L31_ECO_SHR | The processor's data cache was reloaded with Shared (S) data from another core's ECO L3 on the same chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L31_ECO_MOD | The processor's data cache was reloaded with Modified (M) data from another core's ECO L3 on the same chip due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 61
Event | Description
--- | ---
PM_DATA_ALL_FROM_L2MISS_MOD | The processor's data cache was reloaded from a location other than the local core's L2 due to either demand loads or data prefetch
PM_DATA_ALL_FROM_LMEM | The processor's data cache was reloaded from the local chip's Memory due to either demand loads or data prefetch
PM_DATA_ALL_FROM_RMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group ( Remote) due to either demand loads or data prefetch
PM_DATA_ALL_FROM_DMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group (Distant) due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 62
Event | Description
--- | ---
PM_DATA_ALL_FROM_ON_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on the same chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_RL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L21_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L2 on the same chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_L21_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L2 on the same chip due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 63
Event | Description
--- | ---
PM_DATA_ALL_FROM_LL4 | The processor's data cache was reloaded from the local chip's L4 cache due to either demand loads or data prefetch
PM_DATA_ALL_FROM_RL4 | The processor's data cache was reloaded from another chip's L4 on the same Node or Group ( Remote) due to either demand loads or data prefetch
PM_DATA_ALL_FROM_DL4 | The processor's data cache was reloaded from another chip's L4 on a different Node or Group (Distant) due to either demand loads or data prefetch
PM_DATA_ALL_FROM_OFF_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 64
Event | Description
--- | ---
PM_DATA_ALL_FROM_RL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_MEMORY | The processor's data cache was reloaded from a memory location including L4 from local remote or distant due to either demand loads or data prefetch
PM_DATA_ALL_FROM_DL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to either demand loads or data prefetch
PM_DATA_ALL_FROM_DL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 65
Event | Description
--- | ---
PM_INST_FROM_L2 | The processor's Instruction cache was reloaded from local core's L2 due to an instruction fetch (not prefetch)
PM_INST_FROM_L2_MEPF | The processor's Instruction cache was reloaded from local core's L2 hit without dispatch conflicts on Mepf state. due to an instruction fetch (not prefetch)
PM_INST_FROM_L2_DISP_CONFLICT_LDHITST | The processor's Instruction cache was reloaded from local core's L2 with load hit store conflict due to an instruction fetch (not prefetch)
PM_INST_FROM_L2_DISP_CONFLICT_OTHER | The processor's Instruction cache was reloaded from local core's L2 with dispatch conflict due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CY | Run cycles

### Group 66
Event | Description
--- | ---
PM_INST_FROM_L2_NO_CONFLICT | The processor's Instruction cache was reloaded from local core's L2 without conflict due to an instruction fetch (not prefetch)
PM_INST_FROM_L31_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's L3 on the same chip due to an instruction fetch (not prefetch)
PM_INST_FROM_L21_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's L2 on the same chip due to an instruction fetch (not prefetch)
PM_INST_FROM_L21_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's L2 on the same chip due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 67
Event | Description
--- | ---
PM_INST_FROM_L3_NO_CONFLICT | The processor's Instruction cache was reloaded from local core's L3 without conflict due to an instruction fetch (not prefetch)
PM_INST_FROM_L3_MEPF | The processor's Instruction cache was reloaded from local core's L3 without dispatch conflicts hit on Mepf state. due to an instruction fetch (not prefetch)
PM_INST_FROM_L3_DISP_CONFLICT | The processor's Instruction cache was reloaded from local core's L3 with dispatch conflict due to an instruction fetch (not prefetch)
PM_INST_FROM_L3 | The processor's Instruction cache was reloaded from local core's L3 due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 68
Event | Description
--- | ---
PM_INST_FROM_L2MISS | The processor's Instruction cache was reloaded from a location other than the local core's L2 due to an instruction fetch (not prefetch)
PM_INST_FROM_MEMORY | The processor's Instruction cache was reloaded from a memory location including L4 from local remote or distant due to an instruction fetch (not prefetch)
PM_INST_FROM_L3MISS | Marked instruction was reloaded from a location beyond the local chiplet
PM_INST_FROM_L3MISS_MOD | The processor's Instruction cache was reloaded from a location other than the local core's L3 due to a instruction fetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 69
Event | Description
--- | ---
PM_INST_FROM_L31_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's L3 on the same chip due to an instruction fetch (not prefetch)
PM_INST_FROM_RL2L3_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to an instruction fetch (not prefetch)
PM_INST_FROM_L31_ECO_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's ECO L3 on the same chip due to an instruction fetch (not prefetch)
PM_INST_FROM_L31_ECO_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's ECO L3 on the same chip due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 70
Event | Description
--- | ---
PM_INST_FROM_ON_CHIP_CACHE | The processor's Instruction cache was reloaded either shared or modified data from another core's L2/L3 on the same chip due to an instruction fetch (not prefetch)
PM_INST_FROM_LMEM | The processor's Instruction cache was reloaded from the local chip's Memory due to an instruction fetch (not prefetch)
PM_INST_FROM_RMEM | The processor's Instruction cache was reloaded from another chip's memory on the same Node or Group ( Remote) due to an instruction fetch (not prefetch)
PM_INST_FROM_OFF_CHIP_CACHE | The processor's Instruction cache was reloaded either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 71
Event | Description
--- | ---
PM_INST_FROM_RL2L3_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to an instruction fetch (not prefetch)
PM_UTHROTTLE | Cycles in which instruction issue throttle was active in ISU
PM_INST_FROM_DL2L3_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to an instruction fetch (not prefetch)
PM_INST_FROM_DL2L3_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 72
Event | Description
--- | ---
PM_INST_FROM_LL4 | The processor's Instruction cache was reloaded from the local chip's L4 cache due to an instruction fetch (not prefetch)
PM_INST_FROM_RL4 | The processor's Instruction cache was reloaded from another chip's L4 on the same Node or Group ( Remote) due to an instruction fetch (not prefetch)
PM_INST_FROM_DL4 | The processor's Instruction cache was reloaded from another chip's L4 on a different Node or Group (Distant) due to an instruction fetch (not prefetch)
PM_INST_FROM_DMEM | The processor's Instruction cache was reloaded from another chip's memory on the same Node or Group (Distant) due to an instruction fetch (not prefetch)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 73
Event | Description
--- | ---
PM_INST_ALL_FROM_L2 | The processor's Instruction cache was reloaded from local core's L2 due to instruction fetches and prefetches
PM_INST_ALL_FROM_L2_MEPF | The processor's Instruction cache was reloaded from local core's L2 hit without dispatch conflicts on Mepf state. due to instruction fetches and prefetches
PM_INST_ALL_FROM_L2_DISP_CONFLICT_LDHITST | The processor's Instruction cache was reloaded from local core's L2 with load hit store conflict due to instruction fetches and prefetches
PM_INST_ALL_FROM_L2_DISP_CONFLICT_OTHER | The processor's Instruction cache was reloaded from local core's L2 with dispatch conflict due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 74
Event | Description
--- | ---
PM_INST_ALL_FROM_L2_NO_CONFLICT | The processor's Instruction cache was reloaded from local core's L2 without conflict due to instruction fetches and prefetches
PM_INST_ALL_FROM_L31_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's L3 on the same chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_L21_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's L2 on the same chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_L21_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's L2 on the same chip due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 75
Event | Description
--- | ---
PM_INST_ALL_FROM_L3_NO_CONFLICT | The processor's Instruction cache was reloaded from local core's L3 without conflict due to instruction fetches and prefetches
PM_INST_ALL_FROM_L3_MEPF | The processor's Instruction cache was reloaded from local core's L3 without dispatch conflicts hit on Mepf state. due to instruction fetches and prefetches
PM_INST_ALL_FROM_L3_DISP_CONFLICT | The processor's Instruction cache was reloaded from local core's L3 with dispatch conflict due to instruction fetches and prefetches
PM_INST_ALL_FROM_L3 | The processor's Instruction cache was reloaded from local core's L3 due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 76
Event | Description
--- | ---
PM_INST_ALL_FROM_L2MISS | The processor's Instruction cache was reloaded from a location other than the local core's L2 due to instruction fetches and prefetches
PM_INST_ALL_FROM_MEMORY | The processor's Instruction cache was reloaded from a memory location including L4 from local remote or distant due to instruction fetches and prefetches
PM_ISLB_MISS | Instruction SLB Miss Total of all segment sizes
PM_INST_ALL_FROM_L3MISS_MOD | The processor's Instruction cache was reloaded from a location other than the local core's L3 due to a instruction fetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 77
Event | Description
--- | ---
PM_INST_ALL_FROM_L31_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's L3 on the same chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_RL2L3_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_L31_ECO_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another core's ECO L3 on the same chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_L31_ECO_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another core's ECO L3 on the same chip due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 78
Event | Description
--- | ---
PM_INST_ALL_FROM_ON_CHIP_CACHE | The processor's Instruction cache was reloaded either shared or modified data from another core's L2/L3 on the same chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_LMEM | The processor's Instruction cache was reloaded from the local chip's Memory due to instruction fetches and prefetches
PM_INST_ALL_FROM_RMEM | The processor's Instruction cache was reloaded from another chip's memory on the same Node or Group ( Remote) due to instruction fetches and prefetches
PM_INST_ALL_FROM_OFF_CHIP_CACHE | The processor's Instruction cache was reloaded either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 79
Event | Description
--- | ---
PM_INST_ALL_FROM_RL2L3_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to instruction fetches and prefetches
PM_HV_CYC | Cycles in which msr_hv is high. Note that this event does not take msr_pr into consideration
PM_INST_ALL_FROM_DL2L3_SHR | The processor's Instruction cache was reloaded with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to instruction fetches and prefetches
PM_INST_ALL_FROM_DL2L3_MOD | The processor's Instruction cache was reloaded with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 80
Event | Description
--- | ---
PM_INST_ALL_FROM_LL4 | The processor's Instruction cache was reloaded from the local chip's L4 cache due to instruction fetches and prefetches
PM_INST_ALL_FROM_RL4 | The processor's Instruction cache was reloaded from another chip's L4 on the same Node or Group ( Remote) due to instruction fetches and prefetches
PM_INST_ALL_FROM_DL4 | The processor's Instruction cache was reloaded from another chip's L4 on a different Node or Group (Distant) due to instruction fetches and prefetches
PM_INST_ALL_FROM_DMEM | The processor's Instruction cache was reloaded from another chip's memory on the same Node or Group (Distant) due to instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 81
Event | Description
--- | ---
PM_MRK_LD_MISS_EXPOSED_CYC | Marked Load exposed Miss cycles
PM_MRK_LD_MISS_L1 | Marked DL1 Demand Miss counted at exec time
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_LD_MISS_L1_CYC | Marked ld latency
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 82
Event | Description
--- | ---
PM_MRK_DATA_FROM_L2 | The processor's data cache was reloaded from local core's L2 due to a marked load
PM_BR_UNCOND_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred=00 AND if_pc_br0_completed.
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L2_CYC | Duration in cycles to reload from local core's L2 due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 83
Event | Description
--- | ---
PM_BR_PRED_TA_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred(0)='1'.
PM_MRK_DATA_FROM_L2_DISP_CONFLICT_LDHITST_CYC | Duration in cycles to reload from local core's L2 with load hit store conflict due to a marked load
PM_MRK_DATA_FROM_L2_DISP_CONFLICT_LDHITST | The processor's data cache was reloaded from local core's L2 with load hit store conflict due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 84
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L2_DISP_CONFLICT_OTHER_CYC | Duration in cycles to reload from local core's L2 with dispatch conflict due to a marked load
PM_MRK_ST_CMPL_INT | Marked store finished with intervention
PM_MRK_DATA_FROM_L2_DISP_CONFLICT_OTHER | The processor's data cache was reloaded from local core's L2 with dispatch conflict due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 85
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L2_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L2 without conflict due to a marked data side request
PM_MRK_DATA_FROM_L2_MEPF | The processor's data cache was reloaded from local core's L2 hit without dispatch conflicts on Mepf state. due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L2_MEPF_CYC | Duration in cycles to reload from local core's L2 hit without dispatch conflicts on Mepf state. due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 86
Event | Description
--- | ---
PM_MRK_DATA_FROM_L2_NO_CONFLICT | The processor's data cache was reloaded from local core's L2 without conflict due to a marked load
PM_MRK_DATA_FROM_L21_SHR_CYC | Duration in cycles to reload with Shared (S) data from another core's L2 on the same chip due to a marked load
PM_MRK_DATA_FROM_L21_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L2 on the same chip due to a marked load
PM_MRK_DATA_FROM_L2_NO_CONFLICT_CYC | Duration in cycles to reload from local core's L2 without conflict due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 87
Event | Description
--- | ---
PM_DATA_ALL_FROM_L3_NO_CONFLICT | The processor's data cache was reloaded from local core's L3 without conflict due to either demand loads or data prefetch
PM_MRK_DATA_FROM_L3_DISP_CONFLICT_CYC | Duration in cycles to reload from local core's L3 with dispatch conflict due to a marked load
PM_MRK_DATA_FROM_L3_DISP_CONFLICT | The processor's data cache was reloaded from local core's L3 with dispatch conflict due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 88
Event | Description
--- | ---
PM_MRK_DATA_FROM_L2MISS | Data cache reload L2 miss
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_BR_MPRED_CMPL | Marked Branch Mispredicted
PM_MRK_DATA_FROM_L2MISS_CYC | Duration in cycles to reload from a location other than the local core's L2 due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 89
Event | Description
--- | ---
PM_SYNC_MRK_L2MISS | Marked L2 Miss that can throw a synchronous interrupt
PM_MRK_DATA_FROM_L3_CYC | Duration in cycles to reload from local core's L3 due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L3 | The processor's data cache was reloaded from local core's L3 due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 90
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L3_MEPF | The processor's data cache was reloaded from local core's L3 without dispatch conflicts hit on Mepf state. due to a marked load
PM_ST_MISS_L1 | Store Missed L1
PM_MRK_DATA_FROM_L3_MEPF_CYC | Duration in cycles to reload from local core's L3 without dispatch conflicts hit on Mepf state. due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 91
Event | Description
--- | ---
PM_MRK_DATA_FROM_L3_NO_CONFLICT | The processor's data cache was reloaded from local core's L3 without conflict due to a marked load
PM_MRK_DATA_FROM_L31_ECO_SHR_CYC | Duration in cycles to reload with Shared (S) data from another core's ECO L3 on the same chip due to a marked load
PM_MRK_DATA_FROM_L31_ECO_SHR | The processor's data cache was reloaded with Shared (S) data from another core's ECO L3 on the same chip due to a marked load
PM_MRK_DATA_FROM_L3_NO_CONFLICT_CYC | Duration in cycles to reload from local core's L3 without conflict due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 92
Event | Description
--- | ---
PM_SYNC_MRK_L3MISS | Marked L3 misses that can throw a synchronous interrupt
PM_MRK_DATA_FROM_L3MISS_CYC | Duration in cycles to reload from a location other than the local core's L3 due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L3MISS | The processor's data cache was reloaded from a location other than the local core's L3 due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 93
Event | Description
--- | ---
PM_MRK_DATA_FROM_ON_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on the same chip due to a marked load
PM_BACK_BR_CMPL | Branch instruction completed with a target address less than current instruction address
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_ON_CHIP_CACHE_CYC | Duration in cycles to reload either shared or modified data from another core's L2/L3 on the same chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 94
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L21_MOD_CYC | Duration in cycles to reload with Modified (M) data from another core's L2 on the same chip due to a marked load
PM_L1_DCACHE_RELOAD_VALID | DL1 reloaded due to Demand Load
PM_MRK_DATA_FROM_L21_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L2 on the same chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 95
Event | Description
--- | ---
PM_IPTEG_FROM_L2_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L2 without conflict due to a instruction side request
PM_MRK_DATA_FROM_L31_ECO_MOD_CYC | Duration in cycles to reload with Modified (M) data from another core's ECO L3 on the same chip due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L31_ECO_MOD | The processor's data cache was reloaded with Modified (M) data from another core's ECO L3 on the same chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 96
Event | Description
--- | ---
PM_INST_ALL_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for instruction fetches and prefetches
PM_MRK_DATA_FROM_L31_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L3 on the same chip due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L31_MOD_CYC | Duration in cycles to reload with Modified (M) data from another core's L3 on the same chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 97
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_RMEM_CYC | Duration in cycles to reload from another chip's memory on the same Node or Group ( Remote) due to a marked load
PM_MRK_DATA_FROM_RMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group ( Remote) due to a marked load
PM_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, xlate)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 98
Event | Description
--- | ---
PM_MRK_DATA_FROM_L31_SHR | The processor's data cache was reloaded with Shared (S) data from another core's L3 on the same chip due to a marked load
PM_DATA_ALL_FROM_L31_MOD | The processor's data cache was reloaded with Modified (M) data from another core's L3 on the same chip due to either demand loads or data prefetch
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_L31_SHR_CYC | Duration in cycles to reload with Shared (S) data from another core's L3 on the same chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 99
Event | Description
--- | ---
PM_MRK_DATA_FROM_LL4 | The processor's data cache was reloaded from the local chip's L4 cache due to a marked load
PM_MRK_DATA_FROM_DL4_CYC | Duration in cycles to reload from another chip's L4 on a different Node or Group (Distant) due to a marked load
PM_MRK_DATA_FROM_DL4 | The processor's data cache was reloaded from another chip's L4 on a different Node or Group (Distant) due to a marked load
PM_MRK_DATA_FROM_LL4_CYC | Duration in cycles to reload from the local chip's L4 cache due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 100
Event | Description
--- | ---
PM_LD_L3MISS_PEND_CYC | Cycles L3 miss was pending for this thread
PM_MRK_DATA_FROM_LMEM | The processor's data cache was reloaded from the local chip's Memory due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_LMEM_CYC | Duration in cycles to reload from the local chip's Memory due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 101
Event | Description
--- | ---
PM_LD_REF_L1 | All L1 D cache load references counted at finish, gated by reject
PM_MRK_DATA_FROM_MEMORY | The processor's data cache was reloaded from a memory location including L4 from local remote or distant due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_MEMORY_CYC | Duration in cycles to reload from a memory location including L4 from local remote or distant due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 102
Event | Description
--- | ---
PM_INST_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for an instruction fetch
PM_MRK_DATA_FROM_OFF_CHIP_CACHE_CYC | Duration in cycles to reload either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_OFF_CHIP_CACHE | The processor's data cache was reloaded either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 103
Event | Description
--- | ---
PM_INST_ALL_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for instruction fetches and prefetches
PM_MRK_DATA_FROM_RL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_RL2L3_MOD_CYC | Duration in cycles to reload with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 104
Event | Description
--- | ---
PM_MRK_DATA_FROM_RL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked load
PM_MRK_DATA_FROM_DL2L3_SHR_CYC | Duration in cycles to reload with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked load
PM_MRK_DATA_FROM_DL2L3_SHR | The processor's data cache was reloaded with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked load
PM_MRK_DATA_FROM_RL2L3_SHR_CYC | Duration in cycles to reload with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 105
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_RL4 | The processor's data cache was reloaded from another chip's L4 on the same Node or Group ( Remote) due to a marked load
PM_ALL_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_MRK_DATA_FROM_RL4_CYC | Duration in cycles to reload from another chip's L4 on the same Node or Group ( Remote) due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 106
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_DL2L3_MOD_CYC | Duration in cycles to reload with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked load
PM_L2_SYS_PUMP | RC requests that were system pump attempts
PM_MRK_DATA_FROM_DL2L3_MOD | The processor's data cache was reloaded with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 107
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DATA_FROM_DMEM_CYC | Duration in cycles to reload from another chip's memory on the same Node or Group (Distant) due to a marked load
PM_IPTEG_FROM_L2_DISP_CONFLICT_LDHITST | A Page Table Entry was loaded into the TLB from local core's L2 with load hit store conflict due to a instruction side request
PM_MRK_DATA_FROM_DMEM | The processor's data cache was reloaded from another chip's memory on the same Node or Group (Distant) due to a marked load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 108
Event | Description
--- | ---
PM_L1MISS_LAT_EXC_256 | L1 misses that took longer than 256 cycles to resolve (miss to reload)
PM_L1MISS_LAT_EXC_32 | L1 misses that took longer than 32 cycles to resolve (miss to reload)
PM_L1MISS_LAT_EXC_1024 | L1 misses that took longer than 1024 cycles to resolve (miss to reload)
PM_L1MISS_LAT_EXC_2048 | L1 misses that took longer than 2048 cycles to resolve (miss to reload)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 109
Event | Description
--- | ---
PM_MRK_ST_L2DISP_TO_CMPL_CYC | Cycles from L2 RC dispatch to L2 RC completion
PM_MRK_ST_NEST | Marked store sent to nest
PM_MRK_ST_DRAIN_TO_L2DISP_CYC | Cycles to drain st from core to L2
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 110
Event | Description
--- | ---
PM_L2_ST | All successful D side store dispatches for this thread
PM_L2_CHIP_PUMP | RC requests that were local on chip pump attempts
PM_L2_SYS_PUMP | RC requests that were system pump attempts
PM_L2_RTY_LD | RC retries on PB for any load from core
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 111
Event | Description
--- | ---
PM_L2_ST_MISS | All successful D side store dispatches for this thread that were L2 Miss
PM_L2_GROUP_PUMP | RC requests that were on Node Pump attempts
PM_L2_RTY_ST | RC retries on PB for any store from core
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 112
Event | Description
--- | ---
PM_RC_LIFETIME_EXC_256 | Number of times the RC machine for a sampled instruction was active for more than 256 cycles
PM_RC_LIFETIME_EXC_32 | Number of times the RC machine for a sampled instruction was active for more than 32 cycles
PM_RC_LIFETIME_EXC_1024 | Number of times the RC machine for a sampled instruction was active for more than 1024 cycles
PM_RC_LIFETIME_EXC_2048 | Number of times the RC machine for a sampled instruction was active for more than 2048 cycles
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 113
Event | Description
--- | ---
PM_RC0_BUSY | RC mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_SN0_BUSY | SN mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_RC_USAGE | Continuous 16 cycle(2to1) window where this signals rotates thru sampling each L2 RC machine busy. PMU uses this wave to then do 16 cyc count to sample total number of machs running
PM_SN_USAGE | Continuous 16 cycle(2to1) window where this signals rotates thru sampling each L2 SN machine busy. PMU uses this wave to then do 16 cyc count to sample total number of machs running
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 114
Event | Description
--- | ---
PM_CO0_BUSY | CO mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_L1PF_L2MEMACC | Valid when first beat of data comes in for an L1pref where data came from mem(or L4)
PM_CO_USAGE | Continuous 16 cycle(2to1) window where this signals rotates thru sampling each L2 CO machine busy. PMU uses this wave to then do 16 cyc count to sample total number of machs running
PM_ISIDE_L2MEMACC | Valid when first beat of data comes in for an i side fetch where data came from mem(or L4)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 115
Event | Description
--- | ---
PM_RC0_ALLOC | RC mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_SN0_ALLOC | SN mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_L3_SN0_ALLOC | Lifetime, sample of snooper machine 0 valid
PM_L3_RD0_ALLOC | Lifetime, sample of RD machine 0 valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 116
Event | Description
--- | ---
PM_CO0_ALLOC | CO mach 0 Busy. Used by PMU to sample average RC lifetime (mach0 used as sample point)
PM_ST_CMPL | Store completion count
PM_L3_CO0_ALLOC | Lifetime, sample of CO machine 0 valid
PM_L3_PF0_ALLOC | Lifetime, sample of PF machine 0 valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 117
Event | Description
--- | ---
PM_L3_PF_MISS_L3 | L3 Prefetch missed in L3
PM_L3_CO_L31 | L3 CO to L3.1 OR of port 0 and 1 (lossy)
PM_L3_PF_ON_CHIP_CACHE | L3 Prefetch from On chip cache
PM_L3_PF_ON_CHIP_MEM | L3 Prefetch from On chip memory
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 118
Event | Description
--- | ---
PM_L3_LD_PREF | L3 Load Prefetches
PM_L3_CO_MEM | L3 CO to memory OR of port 0 and 1 (lossy)
PM_L3_PF_OFF_CHIP_CACHE | L3 Prefetch from Off chip cache
PM_L3_PF_OFF_CHIP_MEM | L3 Prefetch from Off chip memory
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 119
Event | Description
--- | ---
PM_L3_SN_USAGE | Rotating sample of 8 snoop valids
PM_L3_RD_USAGE | Rotating sample of 16 RD actives
PM_L3_SN0_BUSY | Lifetime, sample of snooper machine 0 valid
PM_L3_RD0_BUSY | Lifetime, sample of RD machine 0 valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 120
Event | Description
--- | ---
PM_L3_CI_USAGE | Rotating sample of 16 CI or CO actives
PM_L3_PF_USAGE | Rotating sample of 32 PF actives
PM_L3_CO0_BUSY | Lifetime, sample of CO machine 0 valid
PM_L3_PF0_BUSY | Lifetime, sample of PF machine 0 valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 121
Event | Description
--- | ---
PM_VSU0_1FLOP | One flop (fadd, fmul, fsub, fcmp, fsel, fabs, fnabs, fres, fsqrte, fneg) operation finished
PM_VSU1_1FLOP | One flop (fadd, fmul, fsub, fcmp, fsel, fabs, fnabs, fres, fsqrte, fneg) operation finished
PM_VSU0_2FLOP | Two flops operation (scalar fmadd, fnmadd, fmsub, fnmsub and DP vector versions of single flop instructions)
PM_VSU1_2FLOP | Two flops operation (scalar fmadd, fnmadd, fmsub, fnmsub and DP vector versions of single flop instructions)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 122
Event | Description
--- | ---
PM_VSU0_4FLOP | Four flops operation (scalar fdiv, fsqrt, DP vector version of fmadd, fnmadd, fmsub, fnmsub, SP vector versions of single flop instructions)
PM_VSU1_4FLOP | Four flops operation (scalar fdiv, fsqrt, DP vector version of fmadd, fnmadd, fmsub, fnmsub, SP vector versions of single flop instructions)
PM_VSU0_8FLOP | Eight flops operation (DP vector versions of fdiv,fsqrt and SP vector versions of fmadd,fnmadd,fmsub,fnmsub)
PM_VSU1_8FLOP | Eight flops operation (DP vector versions of fdiv,fsqrt and SP vector versions of fmadd,fnmadd,fmsub,fnmsub)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 123
Event | Description
--- | ---
PM_VSU0_COMPLEX_ISSUED | Complex VMX instruction issued
PM_VSU1_COMPLEX_ISSUED | Complex VMX instruction issued
PM_VSU0_SIMPLE_ISSUED | Simple VMX instruction issued
PM_VSU1_SIMPLE_ISSUED | Simple VMX instruction issued
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 124
Event | Description
--- | ---
PM_VSU0_DP_FMA | DP vector version of fmadd,fnmadd,fmsub,fnmsub
PM_VSU1_DP_FMA | DP vector version of fmadd,fnmadd,fmsub,fnmsub
PM_VSU0_FMA | Two flops operation (fmadd, fnmadd, fmsub, fnmsub) Scalar instructions only!
PM_VSU1_FMA | Two flops operation (fmadd, fnmadd, fmsub, fnmsub) Scalar instructions only!
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 125
Event | Description
--- | ---
PM_VSU1_DD_ISSUED | 64BIT Decimal Issued
PM_VSU0_DD_ISSUED | 64BIT Decimal Issued
PM_VSU0_CY_ISSUED | Cryptographic instruction RFC02196 Issued
PM_VSU1_CY_ISSUED | Cryptographic instruction RFC02196 Issued
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 126
Event | Description
--- | ---
PM_VSU1_FSQRT_FDIV | Four flops operation (fdiv,fsqrt) Scalar Instructions only!
PM_VSU0_SQ | Store Vector Issued
PM_VSU1_SQ | Store Vector Issued
PM_VSU0_16FLOP | Sixteen flops operation (SP vector versions of fdiv,fsqrt)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 127
Event | Description
--- | ---
PM_VSU0_SINGLE | FPU single precision
PM_VSU1_SINGLE | FPU single precision
PM_VSU0_FIN | VSU0 Finished an instruction
PM_VSU1_FIN | VSU1 Finished an instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 128
Event | Description
--- | ---
PM_VSU0_EX_ISSUED | Direct move 32/64b VRFtoGPR RFC02206 Issued
PM_VSU1_EX_ISSUED | Direct move 32/64b VRFtoGPR RFC02206 Issued
PM_VSU0_DQ_ISSUED | 128BIT Decimal Issued
PM_VSU1_DQ_ISSUED | 128BIT Decimal Issued
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 129
Event | Description
--- | ---
PM_VSU0_VECTOR_DP_ISSUED | Double Precision vector instruction issued on Pipe0
PM_VSU1_VECTOR_DP_ISSUED | Double Precision vector instruction issued on Pipe1
PM_VSU0_VECTOR_SP_ISSUED | Single Precision vector instruction issued (executed)
PM_VSU1_VECTOR_SP_ISSUED | Single Precision vector instruction issued (executed)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 130
Event | Description
--- | ---
PM_VSU0_SCALAR_DP_ISSUED | Double Precision scalar instruction issued on Pipe0
PM_VSU1_SCALAR_DP_ISSUED | Double Precision scalar instruction issued on Pipe1
PM_VSU0_DP_FSQRT_FDIV | DP vector versions of fdiv,fsqrt
PM_VSU1_DP_FSQRT_FDIV | DP vector versions of fdiv,fsqrt
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 131
Event | Description
--- | ---
PM_VSU0_FPSCR | Move to/from FPSCR type instruction issued on Pipe 0
PM_VSU1_FPSCR | Move to/from FPSCR type instruction issued on Pipe 0
PM_VSU0_DP_2FLOP | DP vector version of fmul, fsub, fcmp, fsel, fabs, fnabs, fres ,fsqrte, fneg
PM_VSU1_DP_2FLOP | DP vector version of fmul, fsub, fcmp, fsel, fabs, fnabs, fres ,fsqrte, fneg
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 132
Event | Description
--- | ---
PM_VSU0_STF | FPU store (SP or DP) issued on Pipe0
PM_VSU1_STF | FPU store (SP or DP) issued on Pipe1
PM_VSU0_PERMUTE_ISSUED | Permute VMX Instruction Issued
PM_VSU1_PERMUTE_ISSUED | Permute VMX Instruction Issued
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 133
Event | Description
--- | ---
PM_VSU0_FSQRT_FDIV | Four flops operation (fdiv,fsqrt) Scalar Instructions only!
PM_VSU1_FSQRT_FDIV | Four flops operation (fdiv,fsqrt) Scalar Instructions only!
PM_VSU1_16FLOP | Sixteen flops operation (SP vector versions of fdiv,fsqrt)
PM_IPTEG_FROM_L3 | A Page Table Entry was loaded into the TLB from local core's L3 due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 134
Event | Description
--- | ---
PM_DFU | Finish DFU (all finish)
PM_DFU_DCFFIX | Convert from fixed opcode finish (dcffix,dcffixq)
PM_DFU_DENBCD | BCD >DPD opcode finish (denbcd, denbcdq)
PM_DFU_MC | Finish DFU multicycle
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 135
Event | Description
--- | ---
PM_DPTEG_FROM_L2_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L2 without conflict due to a data side request
PM_DPTEG_FROM_L2_MEPF | A Page Table Entry was loaded into the TLB from local core's L2 hit without dispatch conflicts on Mepf state, due to a data side request
PM_DPTEG_FROM_L2_DISP_CONFLICT_LDHITST | A Page Table Entry was loaded into the TLB from local core's L2 with load hit store conflict due to a data side request
PM_DPTEG_FROM_L2_DISP_CONFLICT_OTHER | A Page Table Entry was loaded into the TLB from local core's L2 with dispatch conflict due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 136
Event | Description
--- | ---
PM_DPTEG_FROM_L3_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 without conflict due to a data side request
PM_DPTEG_FROM_L3_MEPF | A Page Table Entry was loaded into the TLB from local core's L3 without dispatch conflicts hit on Mepf state, due to a data side request
PM_DPTEG_FROM_L3_DISP_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 with dispatch conflict due to a data side request
PM_DPTEG_FROM_L3 | A Page Table Entry was loaded into the TLB from local core's L3 due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 137
Event | Description
--- | ---
PM_DPTEG_FROM_L31_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L3 on the same chip due to a data side request
PM_DPTEG_FROM_L31_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L3 on the same chip due to a data side request
PM_DPTEG_FROM_L31_ECO_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's ECO L3 on the same chip due to a data side request
PM_DPTEG_FROM_L31_ECO_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's ECO L3 on the same chip due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 138
Event | Description
--- | ---
PM_DPTEG_FROM_RL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a data side request
PM_DPTEG_FROM_RL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a data side request
PM_DPTEG_FROM_DL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a data side request
PM_DPTEG_FROM_DL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 139
Event | Description
--- | ---
PM_DPTEG_FROM_ON_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on the same chip due to a data side request
PM_DPTEG_FROM_MEMORY | A Page Table Entry was loaded into the TLB from a memory location including L4 from local remote or distant due to a data side request
PM_DPTEG_FROM_RMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group ( Remote) due to a data side request
PM_DPTEG_FROM_OFF_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 140
Event | Description
--- | ---
PM_DPTEG_FROM_LL4 | A Page Table Entry was loaded into the TLB from the local chip's L4 cache due to a data side request
PM_DPTEG_FROM_RL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on the same Node or Group ( Remote) due to a data side request
PM_DPTEG_FROM_DL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on a different Node or Group (Distant) due to a data side request
PM_DPTEG_FROM_DMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group (Distant) due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 141
Event | Description
--- | ---
PM_DPTEG_FROM_L2MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L2 due to a data side request
PM_DPTEG_FROM_LMEM | A Page Table Entry was loaded into the TLB from the local chip's Memory due to a data side request
PM_DPTEG_FROM_RMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group ( Remote) due to a data side request
PM_DPTEG_FROM_L3MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L3 due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 142
Event | Description
--- | ---
PM_DPTEG_FROM_L2 | A Page Table Entry was loaded into the TLB from local core's L2 due to a data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_DPTEG_FROM_L21_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L2 on the same chip due to a data side request
PM_DPTEG_FROM_L21_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L2 on the same chip due to a data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 143
Event | Description
--- | ---
PM_IPTEG_FROM_L3_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 without conflict due to a instruction side request
PM_IPTEG_FROM_L3_MEPF | A Page Table Entry was loaded into the TLB from local core's L3 without dispatch conflicts hit on Mepf state. due to a instruction side request
PM_IPTEG_FROM_L3_DISP_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 with dispatch conflict due to a instruction side request
PM_IPTEG_FROM_L2_DISP_CONFLICT_OTHER | A Page Table Entry was loaded into the TLB from local core's L2 with dispatch conflict due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 144
Event | Description
--- | ---
PM_IPTEG_FROM_L31_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L3 on the same chip due to a instruction side request
PM_IPTEG_FROM_L31_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L3 on the same chip due to a instruction side request
PM_IPTEG_FROM_L31_ECO_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's ECO L3 on the same chip due to a instruction side request
PM_IPTEG_FROM_L31_ECO_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's ECO L3 on the same chip due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 145
Event | Description
--- | ---
PM_IPTEG_FROM_RL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a instruction side request
PM_IPTEG_FROM_RL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a instruction side request
PM_IPTEG_FROM_DL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a instruction side request
PM_IPTEG_FROM_DL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 146
Event | Description
--- | ---
PM_IPTEG_FROM_ON_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on the same chip due to a instruction side request
PM_IPTEG_FROM_MEMORY | A Page Table Entry was loaded into the TLB from a memory location including L4 from local remote or distant due to a instruction side request
PM_IPTEG_FROM_RMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group ( Remote) due to a instruction side request
PM_IPTEG_FROM_OFF_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 147
Event | Description
--- | ---
PM_IPTEG_FROM_LL4 | A Page Table Entry was loaded into the TLB from the local chip's L4 cache due to a instruction side request
PM_IPTEG_FROM_RL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on the same Node or Group ( Remote) due to a instruction side request
PM_IPTEG_FROM_DL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on a different Node or Group (Distant) due to a instruction side request
PM_IPTEG_FROM_L3MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L3 due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 148
Event | Description
--- | ---
PM_IPTEG_FROM_L2MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L2 due to a instruction side request
PM_IPTEG_FROM_LMEM | A Page Table Entry was loaded into the TLB from the local chip's Memory due to a instruction side request
PM_IPTEG_FROM_RMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group ( Remote) due to a instruction side request
PM_IPTEG_FROM_DMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group (Distant) due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 149
Event | Description
--- | ---
PM_IPTEG_FROM_L2 | A Page Table Entry was loaded into the TLB from local core's L2 due to a instruction side request
PM_IPTEG_FROM_L2_MEPF | A Page Table Entry was loaded into the TLB from local core's L2 hit without dispatch conflicts on Mepf state. due to a instruction side request
PM_IPTEG_FROM_L21_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L2 on the same chip due to a instruction side request
PM_IPTEG_FROM_L21_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L2 on the same chip due to a instruction side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 150
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L2_MEPF | A Page Table Entry was loaded into the TLB from local core's L2 hit without dispatch conflicts on Mepf state. due to a marked data side request
PM_MRK_DPTEG_FROM_L2_DISP_CONFLICT_LDHITST | A Page Table Entry was loaded into the TLB from local core's L2 with load hit store conflict due to a marked data side request
PM_MRK_DPTEG_FROM_L2_DISP_CONFLICT_OTHER | A Page Table Entry was loaded into the TLB from local core's L2 with dispatch conflict due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 151
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L3_NO_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 without conflict due to a marked data side request
PM_MRK_DPTEG_FROM_L3_MEPF | A Page Table Entry was loaded into the TLB from local core's L3 without dispatch conflicts hit on Mepf state. due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L3 | A Page Table Entry was loaded into the TLB from local core's L3 due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 152
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L31_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L3 on the same chip due to a marked data side request
PM_MRK_DPTEG_FROM_L31_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L3 on the same chip due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L31_ECO_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's ECO L3 on the same chip due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 153
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L2 | A Page Table Entry was loaded into the TLB from local core's L2 due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L21_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L2 on the same chip due to a marked data side request
PM_MRK_DPTEG_FROM_L21_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's L2 on the same chip due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 154
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_RL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_DL2L3_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked data side request
PM_MRK_DPTEG_FROM_DL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on a different Node or Group (Distant), as this chip due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 155
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_ON_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on the same chip due to a marked data side request
PM_MRK_DPTEG_FROM_LMEM | A Page Table Entry was loaded into the TLB from the local chip's Memory due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_OFF_CHIP_CACHE | A Page Table Entry was loaded into the TLB either shared or modified data from another core's L2/L3 on a different chip (remote or distant) due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 156
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_LL4 | A Page Table Entry was loaded into the TLB from the local chip's L4 cache due to a marked data side request
PM_MRK_DPTEG_FROM_RL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on the same Node or Group ( Remote) due to a marked data side request
PM_MRK_DPTEG_FROM_DL4 | A Page Table Entry was loaded into the TLB from another chip's L4 on a different Node or Group (Distant) due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 157
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L2MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L2 due to a marked data side request
PM_MRK_DATA_FROM_MEM | The processor's data cache was reloaded from a memory location including L4 from local remote or distant due to a marked load
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L3MISS | A Page Table Entry was loaded into the TLB from a location other than the local core's L3 due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 158
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_MEMORY | A Page Table Entry was loaded into the TLB from a memory location including L4 from local remote or distant due to a marked data side request
PM_MRK_DPTEG_FROM_RMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group ( Remote) due to a marked data side request
PM_MRK_DPTEG_FROM_DMEM | A Page Table Entry was loaded into the TLB from another chip's memory on the same Node or Group (Distant) due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 159
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_RL2L3_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another chip's L2 or L3 on the same Node or Group (Remote), as this chip due to a marked data side request
PM_MRK_DPTEG_FROM_L3_DISP_CONFLICT | A Page Table Entry was loaded into the TLB from local core's L3 with dispatch conflict due to a marked data side request
PM_MRK_DPTEG_FROM_L2_DISP_CONFLICT_OTHER | A Page Table Entry was loaded into the TLB from local core's L2 with dispatch conflict due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 160
Event | Description
--- | ---
PM_MRK_DPTEG_FROM_L31_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's L3 on the same chip due to a marked data side request
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DPTEG_FROM_L31_ECO_SHR | A Page Table Entry was loaded into the TLB with Shared (S) data from another core's ECO L3 on the same chip due to a marked data side request
PM_MRK_DPTEG_FROM_L31_ECO_MOD | A Page Table Entry was loaded into the TLB with Modified (M) data from another core's ECO L3 on the same chip due to a marked data side request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 161
Event | Description
--- | ---
PM_LSU_FX_FIN | LSU Finished a FX operation (up to 2 per cycle)
PM_ST_FIN | Store Instructions Finished
PM_LSU_FIN | LSU Finished an instruction (up to 2 per cycle)
PM_LD_MISS_L1 | Load Missed L1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 162
Event | Description
--- | ---
PM_LSU_LRQ_S0_ALLOC | Per thread use edge detect to count allocates On a per thread basis, level signal indicating Slot 0 is valid. By instrumenting a single slot we can calculate service time for that slot. Previous machines required a separate signal indicating the slot was allocated. Because any signal can be routed to any counter in P8, we can count level in one PMC and edge detect in another PMC using the same signal
PM_LSU_LRQ_S43_VALID | LRQ slot 43 was busy
PM_LSU_LRQ_S43_ALLOC | LRQ slot 43 was released
PM_LSU_LRQ_S0_VALID | Slot 0 of LRQ valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 163
Event | Description
--- | ---
PM_LSU_LMQ_FULL_CYC | LMQ full
PM_LSU_LMQ_SRQ_EMPTY_CYC | LSU empty (lmq and srq empty)
PM_LSU_LMQ_S0_VALID | Per thread use edge detect to count allocates On a per thread basis, level signal indicating Slot 0 is valid. By instrumenting a single slot we can calculate service time for that slot. Previous machines required a separate signal indicating the slot was allocated. Because any signal can be routed to any counter in P8, we can count level in one PMC and edge detect in another PMC using the same signal.
PM_LSU_LMQ_S0_ALLOC | Slot 0 of LMQ valid
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 164
Event | Description
--- | ---
PM_LSU_SRQ_S39_ALLOC | SRQ slot 39 was released
PM_LSU_SRQ_S0_VALID | Slot 0 of SRQ valid
PM_LSU_SRQ_S0_ALLOC | Per thread use edge detect to count allocates On a per thread basis, level signal indicating Slot 0 is valid. By instrumenting a single slot we can calculate service time for that slot. Previous machines required a separate signal indicating the slot was allocated. Because any signal can be routed to any counter in P8, we can count level in one PMC and edge detect in another PMC using the same signal.
PM_LSU_SRQ_S39_VALID | SRQ slot 39 was busy
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 165
Event | Description
--- | ---
PM_LSU_SRQ_FULL_CYC | Storage Queue is full and is blocking dispatch
PM_REAL_SRQ_FULL | Out of real srq entries
PM_DISP_HELD_SRQ_FULL | Dispatch held due SRQ no room
PM_LSU0_STORE_REJECT | LS0 store reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 166
Event | Description
--- | ---
PM_LSU0_LMQ_LHR_MERGE | LS0 Load Merged with another cacheline request
PM_LSU1_LMQ_LHR_MERGE | LS1 Load Merge with another cacheline request
PM_LSU2_LMQ_LHR_MERGE | LS0 Load Merged with another cacheline request
PM_LSU3_LMQ_LHR_MERGE | LS1 Load Merge with another cacheline request
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 167
Event | Description
--- | ---
PM_LD_REF_L1_LSU0 | LS0 L1 D cache load references counted at finish, gated by reject
PM_LD_REF_L1_LSU1 | LS1 L1 D cache load references counted at finish, gated by reject
PM_LD_REF_L1_LSU2 | LS2 L1 D cache load references counted at finish, gated by reject
PM_LD_REF_L1_LSU3 | LS3 L1 D cache load references counted at finish, gated by reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 168
Event | Description
--- | ---
PM_LS0_L1_PREF | LS0 L1 cache data prefetches
PM_LS1_L1_PREF | LS1 L1 cache data prefetches
PM_LS0_L1_SW_PREF | Software L1 Prefetches, including SW Transient Prefetches
PM_LS1_L1_SW_PREF | Software L1 Prefetches, including SW Transient Prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 169
Event | Description
--- | ---
PM_LSU0_SRQ_STFWD | LS0 SRQ forwarded data to a load
PM_LSU1_SRQ_STFWD | LS1 SRQ forwarded data to a load
PM_LSU2_SRQ_STFWD | LS2 SRQ forwarded data to a load
PM_LSU3_SRQ_STFWD | LS3 SRQ forwarded data to a load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 170
Event | Description
--- | ---
PM_LSU0_FLUSH_UST | LS0 Flush: Unaligned Store
PM_LSU1_FLUSH_UST | LS1 Flush: Unaligned Store
PM_FREQ_DOWN | Power® Management: Below Threshold B
PM_FREQ_UP | Power Management: Above Threshold A
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 171
Event | Description
--- | ---
PM_LSU0_FLUSH_LRQ | LS0 Flush: LRQ
PM_LSU1_FLUSH_LRQ | LS1 Flush: LRQ
PM_LSU2_FLUSH_LRQ | LS2 Flush: LRQ
PM_LSU3_FLUSH_LRQ | LS3 Flush: LRQ
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 172
Event | Description
--- | ---
PM_LSU0_FLUSH_ULD | LS0 Flush: Unaligned Load
PM_LSU1_FLUSH_ULD | LS1 Flush: Unaligned Load
PM_LSU2_FLUSH_ULD | LS3 Flush: Unaligned Load
PM_LSU3_FLUSH_ULD | LS14 Flush: Unaligned Load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 173
Event | Description
--- | ---
PM_LSU0_FLUSH_SRQ | LS0 Flush: SRQ
PM_LSU1_FLUSH_SRQ | LS1 Flush: SRQ
PM_LSU2_FLUSH_SRQ | LS2 Flush: SRQ
PM_LSU3_FLUSH_SRQ | LS13 Flush: SRQ
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 174
Event | Description
--- | ---
PM_PTE_PREFETCH | PTE prefetches
PM_TABLEWALK_CYC_PREF | Tablewalk qualified for pte prefetches
PM_LSU_FOUR_TABLEWALK_CYC | Cycles when four tablewalks pending on this thread
PM_LSU_TWO_TABLEWALK_CYC | Cycles when two tablewalks pending on this thread
RUN_INST_CMPL Run instructions
PM_RUN_CYC | Run cycles

### Group 175
Event | Description
--- | ---
PM_LSU0_PRIMARY_ERAT_HIT | Primary ERAT hit
PM_LSU1_PRIMARY_ERAT_HIT | Primary ERAT hit
PM_LSU2_PRIMARY_ERAT_HIT | Primary ERAT hit
PM_LSU3_PRIMARY_ERAT_HIT | Primary ERAT hit
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 176
Event | Description
--- | ---
PM_LSU2_LDF | LS2 Scalar Loads
PM_LSU3_LDF | LS3 Scalar Loads
PM_LSU2_LDX | LS0 Vector Loads
PM_LSU3_LDX | LS1 Vector Loads
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 177
Event | Description
--- | ---
PM_LSU0_NCLD | LS0 Non cachable Loads counted at finish
PM_LSU1_NCLD | LS1 Non cachable Loads counted at finish
PM_LSU_NCST | Non cachable Stores sent to nest
PM_SNOOP_TLBIE | TLBIE snoop
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 178
Event | Description
--- | ---
PM_DSLB_MISS | Data SLB Misses, total of all segment sizes
PM_ISLB_MISS | Instruction SLB Miss Total of all segment sizes
PM_LS0_ERAT_MISS_PREF | LS0 Erat miss due to prefetch
PM_LS1_ERAT_MISS_PREF | LS1 Erat miss due to prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 179
Event | Description
--- | ---
PM_LSU_SRQ_SYNC | A sync in the SRQ ended
PM_LSU_SET_MPRED | Line already in cache at reload time
PM_LSU_SRQ_SYNC_CYC | A sync is in the SRQ (edge detect to count)
PM_SEC_ERAT_HIT | Secondary ERAT Hit
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 180
Event | Description
--- | ---
PM_UP_PREF_L3 | Micropartition prefetch
PM_UP_PREF_POINTER | Micropartition pointer prefetches
PM_DC_COLLISIONS | DATA Cache collisions
PM_TEND_PEND_CYC | TEND latency per thread
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 181
Event | Description
--- | ---
PM_TM_FAIL_DISALLOW | TM fail disallow
PM_TM_FAIL_TLBIE | TLBIE hit bloom filter
PM_TM_FAIL_TX_CONFLICT | Transactional conflict from LSU, whatever gets reported to TEXAS
PM_TM_FAIL_NON_TX_CONFLICT | Non transactional conflict from LSU, whatever gets reported to TEXAS
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 182
Event | Description
--- | ---
PM_LSU0_TM_L1_HIT | Load TM hit in L1
PM_LSU1_TM_L1_HIT | Load TM hit in L1
PM_LSU2_TM_L1_HIT | Load TM hit in L1
PM_LSU3_TM_L1_HIT | Load TM hit in L1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 183
Event | Description
--- | ---
PM_LSU0_TM_L1_MISS | Load TM L1 miss
PM_LSU1_TM_L1_MISS | Load TM L1 miss
PM_LSU2_TM_L1_MISS | Load TM L1 miss
PM_LSU3_TM_L1_MISS | Load TM L1 miss
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 184
Event | Description
--- | ---
PM_LSU0_TMA_REQ_L2 | Addrs only req to L2 only on the first one indication that Load footprint is not expanding
PM_LSU1_TMA_REQ_L2 | Address only req to L2 only on the first one, indication that Load footprint is not expanding
PM_LSU2_TMA_REQ_L2 | Addrs only req to L2 only on the first one, indication that Load footprint is not expanding
PM_LSU3_TMA_REQ_L2 | Addrs only req to L2 only on the first one, indication that Load footprint is not expanding
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 185
Event | Description
--- | ---
PM_LSU0_L1_CAM_CANCEL | LS0 L1 tm cam cancel
PM_LSU1_L1_CAM_CANCEL | LS1 L1 TM cam cancel
PM_LSU2_L1_CAM_CANCEL | LS2 L1 TM cam cancel
PM_LSU3_L1_CAM_CANCEL | LS3 L1 TM cam cancel
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 186
Event | Description
--- | ---
PM_TM_FAIL_CON_TM | TEXAS fail reason @ completion
PM_FAV_TBEGIN | Dispatch time Favored tbegin
PM_TM_FAIL_FOOTPRINT_OVERFLOW | TEXAS fail reason @ completion
PM_TM_FAIL_SELF | TEXAS fail reason @ completion
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 187
Event | Description
--- | ---
PM_TM_TRANS_RUN_CYC | Run cycles in transactional state
PM_TM_TRESUME | TM resume
PM_TM_TRANS_RUN_INST | Instructions completed in transactional state
PM_TM_TSUSPEND | TM suspend
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 188
Event | Description
--- | ---
PM_TM_TBEGIN | TM nested tbegin
PM_TM_TX_PASS_RUN_CYC | Cycles spent in successful transactions
PM_TM_TRANS_RUN_INST | Instructions completed in transactional state
PM_TM_TX_PASS_RUN_INST | Run instructions spent in successful transactions.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 189
Event | Description
--- | ---
PM_TM_FAIL_CONF_NON_TM | TEXAS fail reason @ completion
PM_TM_BEGIN_ALL | TM any tbegin
PM_L2_TM_ST_ABORT_SISTER | TM marked store abort
PM_TM_END_ALL | TM any tend
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 190
Event | Description
--- | ---
PM_NESTED_TEND | Completion time nested tend
PM_NON_FAV_TBEGIN | Dispatch time non favored tbegin
PM_OUTER_TBEGIN | Completion time outer tbegin
PM_OUTER_TEND | Completion time outer tend
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 191
Event | Description
--- | ---
PM_MEM_READ | Reads from Memory from this lpar (includes data/instruction/translate/l1 prefetch/instruction prefetch). Includes L4
PM_MEM_PREF | Memory prefetch for this lpar. Includes L4
PM_MEM_RWITM | Memory rwitm for this lpar
PM_MEM_CO | Memory castouts from this lpar
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 192
Event | Description
--- | ---
PM_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_GRP_PUMP_CPRED | Initial and Final Pump Scope and data sourced across this scope was group pump for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, xlate)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 193
Event | Description
--- | ---
PM_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for all data types excluding data prefetch (demand load/instruction prefetch/instruction fetch/translate)
PM_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, xlate)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 194
Event | Description
--- | ---
PM_DATA_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for a demand load
PM_DATA_GRP_PUMP_CPRED | Initial and Final Pump Scope was group pump (prediction=correct) for a demand load
PM_DATA_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump (prediction=correct) for a demand load
PM_DATA_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 195
Event | Description
--- | ---
PM_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for all data types excluding data prefetch (demand load, instruction prefetch, instruction fetch, translate)
PM_DATA_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for a demand load
PM_DATA_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for a demand load
PM_DATA_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for a demand load
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 196
Event | Description
--- | ---
PM_INST_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for an instruction fetch
PM_INST_GRP_PUMP_CPRED | Initial and Final Pump Scope was group pump (prediction=correct) for an instruction fetch
PM_INST_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump (prediction=correct) for an instruction fetch
PM_INST_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for an instruction fetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 197
Event | Description
--- | ---
PM_INST_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for an instruction fetch
PM_INST_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for an instruction fetch
PM_INST_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for an instruction fetch
PM_INST_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for an instruction fetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 198
Event | Description
--- | ---
PM_ALL_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_ALL_GRP_PUMP_CPRED | Initial and Final Pump Scope and data sourced across this scope was group pump for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_ALL_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_ALL_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 199
Event | Description
--- | ---
PM_ALL_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate).
PM_ALL_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_ALL_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_ALL_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 200
Event | Description
--- | ---
PM_DATA_ALL_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for either demand loads or data prefetch
PM_DATA_ALL_GRP_PUMP_CPRED | Initial and Final Pump Scope was group pump (prediction=correct) for either demand loads or data prefetch
PM_DATA_ALL_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump (prediction=correct) for either demand loads or data prefetch
PM_DATA_ALL_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 201
Event | Description
--- | ---
PM_ALL_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for all data types (demand load, data prefetch, instruction prefetch, instruction fetch, translate)
PM_DATA_ALL_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for either demand loads or data prefetch
PM_DATA_ALL_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for either demand loads or data prefetch
PM_DATA_ALL_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for either demand loads or data prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 202
Event | Description
--- | ---
PM_INST_ALL_CHIP_PUMP_CPRED | Initial and Final Pump Scope was chip pump (prediction=correct) for instruction fetches and prefetches
PM_INST_ALL_GRP_PUMP_CPRED | Initial and Final Pump Scope was group pump (prediction=correct) for instruction fetches and prefetches
PM_INST_ALL_SYS_PUMP_CPRED | Initial and Final Pump Scope was system pump (prediction=correct) for instruction fetches and prefetches
PM_INST_ALL_SYS_PUMP_MPRED_RTY | Final Pump Scope (system) ended up larger than Initial Pump Scope (Chip/Group) for instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 203
Event | Description
--- | ---
PM_INST_ALL_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for instruction fetches and prefetches
PM_INST_ALL_GRP_PUMP_MPRED | Final Pump Scope (Group) ended up either larger or smaller than Initial Pump Scope for instruction fetches and prefetches
PM_INST_ALL_SYS_PUMP_MPRED | Final Pump Scope (system) mispredicted. Either the original scope was too small (Chip/Group) or the original scope was System and it should have been smaller. Counts for instruction fetches and prefetches
PM_INST_ALL_PUMP_MPRED | Pump misprediction. Counts across all types of pumps for instruction fetches and prefetches
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 204
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_FAB_RSP_BKILL_CYC | Cycles L2 RC took for a bkill
PM_MRK_FAB_RSP_CLAIM_RTY | Sampled store did a rwitm and got a rty
PM_MRK_FAB_RSP_BKILL | Marked store had to do a bkill
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 205
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_FAB_RSP_DCLAIM_CYC | Cycles L2 RC took for a dclaim
PM_MRK_FAB_RSP_DCLAIM | Marked store had to do a dclaim
PM_MRK_FAB_RSP_RD_RTY | Sampled L2 reads retry count
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 206
Event | Description
--- | ---
PM_MRK_FAB_RSP_RD_T_INTV | Sampled Read got a T intervention
PM_MRK_FAB_RSP_RWITM_RTY | Sampled store did a rwitm and got a rty
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_FAB_RSP_RWITM_CYC | Cycles L2 RC took for a rwitm
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 207
Event | Description
--- | ---
PM_LSU0_LARX_FIN | Larx finished in LSU pipe0
PM_LSU1_LARX_FIN | Larx finished in LSU pipe1
PM_LSU2_LARX_FIN | Larx finished in LSU pipe2
PM_LSU3_LARX_FIN | Larx finished in LSU pipe3
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 208
Event | Description
--- | ---
PM_STCX_FAIL | STCX failed
PM_STCX_LSU | STCX executed reported at sent to nest
PM_LARX_FIN | Larx finished
PM_LSU_SRQ_EMPTY_CYC | ALL threads srq empty
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 209
Event | Description
--- | ---
PM_IERAT_RELOAD | Number of I ERAT reloads
PM_IERAT_RELOAD_4K | IERAT Miss
PM_IERAT_RELOAD_64K | IERAT Reloaded (Miss) for a 64k page
PM_IERAT_RELOAD_16M | IERAT Reloaded (Miss) for a 16M page
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 210
Event | Description
--- | ---
PM_CYC | Cycles
PM_LSU_DERAT_MISS | DERAT Reloaded due to a DERAT miss
PM_DTLB_MISS | Data PTEG reload
PM_ITLB_MISS | ITLB Reloaded
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 211
Event | Description
--- | ---
PM_DATA_PUMP_CPRED | Pump prediction correct. Counts across all types of pumps for a demand load
PM_TLB_MISS | TLB Miss (I + D)
PM_TLBIE_FIN | TLBIE finished
PM_LSU_ERAT_MISS_PREF | Erat miss due to prefetch, on either pipe
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 212
Event | Description
--- | ---
PM_TABLEWALK_CYC | Cycles when a tablewalk (I or D) is active
PM_TABLEWALK_CYC_PREF | Tablewalk qualified for pte prefetches
PM_DATA_TABLEWALK_CYC | Tablwalk Cycles (could be 1 or 2 active)
PM_LD_MISS_L1 | Load Missed L1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 213
Event | Description
--- | ---
PM_INST_IMC_MATCH_CMPL | IMC Match Count ( Not architected in P8)
PM_INST_FROM_L1 | Instruction fetches from L1
PM_INST_IMC_MATCH_DISP | Matched Instructions Dispatched
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 214
Event | Description
--- | ---
PM_EE_OFF_EXT_INT | EE off and external interrupt
PM_EXT_INT | External interrupt
PM_TB_BIT_TRANS | Timebase event
PM_CYC | Cycles
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 215
Event | Description
--- | ---
PM_L1_DEMAND_WRITE | Instruction demand sectors written into IL1
PM_IC_PREF_WRITE | Instruction prefetch written into IL1
PM_IBUF_FULL_CYC | Cycles No room in ibuff
PM_BANK_CONFLICT | Read blocked due to interleave conflict. The ifar logic will detect an interleave conflict and kill the data that was read that cycle.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 216
Event | Description
--- | ---
PM_IC_DEMAND_L2_BHT_REDIRECT | L2 I cache demand request due to BHT redirect, branch redirect ( 2 bubbles 3 cycles)
PM_IC_DEMAND_L2_BR_REDIRECT | L2 I cache demand request due to branch Mispredict (15 cycle path)
PM_IC_DEMAND_REQ | Demand Instruction fetch request
PM_IC_INVALIDATE | Ic line invalidated
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 217
Event | Description
--- | ---
PM_GRP_IC_MISS_NONSPEC | Group experienced non speculative I cache miss
PM_L1_ICACHE_MISS | Demand iCache Miss
PM_IC_RELOAD_PRIVATE | Reloading line was brought in private for a specific thread. Most lines are brought in shared for all eight threads. If RA does not match then invalidates and then brings it shared to other thread. In P7 line brought in private, then line was invalidate
PM_LSU_L1_SW_PREF | Software L1 Prefetches, including SW Transient Prefetches, on both pipes
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 218
Event | Description
--- | ---
PM_IC_PREF_CANCEL_HIT | Prefetch canceled due to icache hit
PM_IC_PREF_CANCEL_L2 | L2 Squashed request
PM_IC_PREF_CANCEL_PAGE | Prefetch canceled due to page boundary
PM_IC_PREF_REQ | Instruction prefetch requests
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 219
Event | Description
--- | ---
PM_DATA_ALL_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for either demand loads or data prefetch
PM_ST_FWD | Store forwards that finished
PM_L1_ICACHE_RELOADED_PREF | Counts all Icache prefetch reloads ( includes demand turned into prefetch)
PM_L1_ICACHE_RELOADED_ALL | Counts all Icache reloads includes demand, prefetchm prefetch turned into demand and demand turned into prefetch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 220
Event | Description
--- | ---
PM_1LPAR_CYC | Number of cycles in single lpar mode. All threads in the core are assigned to the same lpar.
PM_2LPAR_CYC | Cycles in 2 lpar mode. Threads 0 3 belong to Lpar0 and threads 4 7 belong to Lpar1
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_4LPAR_CYC | Number of cycles in 4 LPAR mode. Threads 0 1 belong to lpar0, threads 2 3 belong to lpar1, threads 4 5 belong to lpar2, and threads 6 7 belong to lpar3
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 221
Event | Description
--- | ---
PM_FUSION_TOC_GRP0_1 | One pair of instructions fused with TOC in Group0
PM_FUSION_TOC_GRP0_2 | Two pairs of instructions fused with TOCin Group0
PM_FUSION_TOC_GRP0_3 | Three pairs of instructions fused with TOC in Group0
PM_FUSION_TOC_GRP1_1 | One pair of instructions fused with TOX in Group1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 222
Event | Description
--- | ---
PM_FUSION_VSX_GRP0_1 | One pair of instructions fused with VSX in Group0
PM_FUSION_VSX_GRP0_2 | Two pairs of instructions fused with VSX in Group0
PM_FUSION_VSX_GRP0_3 | Three pairs of instructions fused with VSX in Group0
PM_FUSION_VSX_GRP1_1 | One pair of instructions fused with VSX in Group1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 223
Event | Description
--- | ---
PM_GCT_UTIL_1_2_ENTRIES | GCT Utilization 1 2 entries
PM_GCT_UTIL_3_6_ENTRIES | GCT Utilization 3 6 entries
PM_GCT_UTIL_7_10_ENTRIES | GCT Utilization 7 10 entries
PM_GCT_UTIL_11_14_ENTRIES | GCT Utilization 11 14 entries
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 224
Event | Description
--- | ---
PM_GCT_UTIL_15_17_ENTRIES | GCT Utilization 15 17 entries
PM_GCT_UTIL_18_ENTRIES | GCT Utilization 18+ entries
PM_DISP_HOLD_GCT_FULL | Dispatch Hold Due to no space in the GCT
PM_GCT_MERGE | Group dispatched on a merged GCT empty. GCT entries can be merged only within the same thread
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 225
Event | Description
--- | ---
PM_STALL_END_GCT_EMPTY | Count ended because GCT went empty
PM_GCT_EMPTY_CYC | No itags assigned either thread (GCT Empty)
PM_CYC | Cycles
PM_FLUSH_DISP | Dispatch flush
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 226
Event | Description
--- | ---
PM_FPU0_FCONV | Convert instruction executed
PM_FPU0_FEST | Estimate instruction executed
PM_FPU0_FRSP | Round to single precision instruction executed
PM_LSU_LDF | FPU loads only on LS2/LS3 ie LU0/LU1
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 227
Event | Description
--- | ---
PM_FPU1_FCONV | Convert instruction executed
PM_FPU1_FEST | Estimate instruction executed
PM_FPU1_FRSP | Round to single precision instruction executed
PM_LSU_LDX | Vector loads can issue only on LS2/LS3
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 228
Event | Description
--- | ---
PM_GRP_NON_FULL_GROUP | GROUPs where we did not have 6 non branch instructions in the group(ST mode), in SMT mode 3 non branches
PM_GRP_TERM_2ND_BRANCH | There were enough instructions in the Ibuffer, but 2nd branch ends group
PM_GRP_TERM_FPU_AFTER_BR | There were enough instructions in the Ibuffer, but FPU OP IN same group after a branch terminates a group, cant do partial flushes
PM_GRP_TERM_NOINST | Do not fill every slot in the group, Not enough instructions in the Ibuffer. This includes cases where the group started with enough instructions, but some got knocked out by a cache miss or branch redirect (which would also empty the Ibuffer).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 229
Event | Description
--- | ---
PM_SHL_CREATED | Store Hit Load Table Entry Created
PM_SHL_ST_CONVERT | Store Hit Load Table Read Hit with entry Enabled
PM_SHL_ST_DISABLE | Store Hit Load Table Read Hit with entry Disabled (entry was disabled due to the entry shown to not prevent the flush)
PM_EAT_FULL_CYC | Cycles No room in EAT
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 230
Event | Description
--- | ---
PM_GRP_BR_MPRED_NONSPEC | Group experienced non speculative branch redirect
PM_GRP_TERM_OTHER | There were enough instructions in the Ibuffer, but the group terminated early for some other reason, most likely due to a First or Last.
PM_GRP_TERM_SLOT_LIMIT | There were enough instructions in the Ibuffer, but 3 src RA/RB/RC , 2 way crack caused a group termination
PM_EAT_FORCE_MISPRED | XL form branch was mispredicted due to the predicted target address missing from EAT. The EAT forces a mispredict in this case since there is no predicated target to validate. This is a rare case that may occur when the EAT is full and a branch is issue
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 231
Event | Description
--- | ---
PM_CLB_HELD | CLB Hold: Any Reason
PM_LINK_STACK_INVALID_PTR | A flush were LS ptr is invalid, results in a pop , A lot of interrupts between push and pops
PM_LINK_STACK_WRONG_ADD_PRED | Link stack predicts wrong address, because of link stack design limitation
PM_ISU_REF_FXU | FXU ISU reject from either pipe
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 232
Event | Description
--- | ---
PM_DATA_GRP_PUMP_MPRED_RTY | Final Pump Scope (Group) ended up larger than Initial Pump Scope (Chip) for a demand load
PM_UTHROTTLE | Cycles in which instruction issue throttle was active in ISU
PM_IFETCH_THROTTLE | Cycles in which Instruction fetch throttle was active
PM_IFU_L2_TOUCH | L2 touch to update MRU on a line
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 233
Event | Description
--- | ---
PM_ISU_REJECTS_ALL | All isu rejects could be more than 1 per cycle
PM_ISU_REJECT_SAR_BYPASS | Reject because of SAR bypass
PM_ISU_REJECT_SRC_NA | ISU reject due to source not available
PM_ISU_REJECT_RES_NA | ISU reject due to resource not available
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 234
Event | Description
--- | ---
PM_LSU0_STORE_REJECT | LS0 store reject
PM_LSU1_STORE_REJECT | LS1 store reject
PM_LSU2_REJECT | LSU2 reject
PM_LSU_REJECT_LHS | LSU Reject due to LHS (up to 4 per cycle)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 235
Event | Description
--- | ---
PM_LSU_REJECT_LMQ_FULL | LSU reject due to LMQ full ( 4 per cycle)
PM_LSU_REJECT_ERAT_MISS | LSU Reject due to ERAT (up to 4 per cycles)
PM_LSU2_REJECT | LSU2 reject
PM_LSU3_REJECT | LSU3 reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 236
Event | Description
--- | ---
PM_LSU_REJECT | LSU Reject (up to 4 per cycle)
PM_LSU1_REJECT | LSU1 reject
PM_MRK_LSU_REJECT_ERAT_MISS | LSU marked reject due to ERAT (up to 2 per cycle)
PM_MRK_LSU_REJECT | LSU marked reject (up to 2 per cycle)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 237
Event | Description
--- | ---
PM_ISU_REF_FX0 | FX0 ISU reject
PM_ISU_REF_LS0 | LS0 ISU reject
PM_ISU_REF_LS1 | LS1 ISU reject
PM_ISU_REF_LS2 | LS2 ISU reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 238
Event | Description
--- | ---
PM_ISU_REF_LS3 | LS3 ISU reject
PM_ISU_REJ_VS0 | VS0 ISU reject
PM_ISU_REJ_VS1 | VS1 ISU reject
PM_ISU_REF_FX1 | FX1 ISU reject
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 239
Event | Description
--- | ---
PM_SWAP_CANCEL | SWAP cancel, rtag not available
PM_SWAP_CANCEL_GPR | SWAP cancel , rtag not available for gpr
PM_SWAP_COMPLETE | SWAP cast in completed
PM_SWAP_COMPLETE_GPR | SWAP cast in completed fpr gpr
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 240
Event | Description
--- | ---
PM_MEM_LOC_THRESH_LSU_MED | Local memory above threshold for data prefetch
PM_CASTOUT_ISSUED | Castouts issued
PM_CASTOUT_ISSUED_GPR | Castouts issued GPR
PM_MEM_LOC_THRESH_LSU_HIGH | Local memory above threshold for LSU medium
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 241
Event | Description
--- | ---
PM_TEND_PEND_CYC | TEND latency per thread
PM_TABORT_TRECLAIM | Completion time tabortnoncd, tabortcd, treclaim
PM_LSU_NCLD | Count at finish so can return only on LS0 or LS1
PM_CYC | Cycles
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 242
Event | Description
--- | ---
PM_ISYNC | Isync count per thread
PM_LWSYNC | Threaded version, IC Misses where we got EA dir hit but no sector valids were on. ICBI took line out.
PM_LWSYNC_HELD | LWSYNC held at dispatch
PM_FLUSH_DISP_SYNC | Dispatch Flush: Sync
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 243
Event | Description
--- | ---
PM_MRK_INST_ISSUED | Marked instruction issued
PM_MRK_INST_DECODED | Marked instruction decoded
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_INST_FROM_L3MISS | Marked instruction was reloaded from a location beyond the local chiplet
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 244
Event | Description
--- | ---
PM_MRK_INST_DISP | The thread has dispatched a randomly sampled marked instruction
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_INST_FIN | Marked instruction finished
PM_MRK_INST_TIMEO | Marked Instruction finish timeout (instruction lost)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 245
Event | Description
--- | ---
PM_GRP_MRK | Instruction Marked
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_LSU_MRK_DERAT_MISS | DERAT Reloaded (Miss)
PM_MRK_GRP_CMPL | Marked instruction finished (completed)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 246
Event | Description
--- | ---
PM_GRP_MRK | Instruction Marked
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_GRP_NTC | Marked group ntc cycles.
PM_MRK_GRP_IC_MISS | Marked Group experienced I cache miss
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 247
Event | Description
--- | ---
PM_MRK_L1_ICACHE_MISS | Sampled Instruction suffered an icache Miss
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MULT_MRK | Multiple marked instructions
PM_MRK_INST_TIMEO | Marked Instruction finish timeout (instruction lost)
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 248
Event | Description
--- | ---
PM_MRK_BR_CMPL | Branch Instruction completed
PM_MRK_BRU_FIN | BRU marked instruction finish
PM_MRK_BACK_BR_CMPL | Marked branch instruction completed with a target address less than current instruction address
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 249
Event | Description
--- | ---
PM_MRK_BR_TAKEN_CMPL | Marked Branch Taken completed
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_BR_MPRED_CMPL | Marked Branch Mispredicted
PM_BR_MRK_2PATH | Marked two path branch
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 250
Event | Description
--- | ---
PM_SYNC_MRK_BR_LINK | Marked Branch and link branch that can cause a synchronous interrupt
PM_BR_PRED_LSTACK_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred(0) AND (not if_pc_br0_pred_type).
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_INST_CMPL | Marked instruction completed
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 251
Event | Description
--- | ---
PM_SYNC_MRK_BR_MPRED | Marked Branch mispredict that can cause a synchronous interrupt
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_BR_PRED_CR_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred(1)='1'.
PM_BR_PRED_CCACHE_CMPL | Completion Time Event. This event can also be calculated from the direct bus as follows: if_pc_br0_br_pred(0) AND if_pc_br0_pred_type.
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 252
Event | Description
--- | ---
PM_MRK_ST_CMPL | Marked store completed and sent to nest
PM_MRK_L2_RC_DISP | Marked Instruction RC dispatched in L2
PM_MRK_ST_FWD | Marked st forwards
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 253
Event | Description
--- | ---
PM_MRK_LSU_FLUSH_LRQ | Flush: (marked) LRQ
PM_MRK_LSU_FLUSH_SRQ | Flush: (marked) SRQ
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_LSU_FLUSH_UST | Unaligned Store Flush on either pipe
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 254
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_LSU_FLUSH | Flush: (marked) : All Cases
PM_MRK_LSU_FLUSH_ULD | Flush: (marked) Unaligned Load
PM_MRK_LSU_FLUSH_UST | Flush: (marked) Unaligned Store
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 255
Event | Description
--- | ---
PM_MRK_FIN_STALL_CYC | Marked instruction Finish Stall cycles (marked finish after NTC) (use edge detect to count)
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_STCX_FAIL | Marked stcx failed
PM_MRK_LARX_FIN | Larx finished
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 256
Event | Description
--- | ---
PM_MRK_RUN_CYC | Marked run cycles
PM_MRK_DFU_FIN | Decimal Unit marked Instruction Finish
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_LSU_FIN | LSU marked instr finish
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 257
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_FXU_FIN | FXU marked instruction finish
PM_MRK_CRU_FIN | IFU non branch finished
PM_CRU_FIN | IFU Finished a (non branch) instruction
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 258
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_NTF_FIN | Marked next to finish instruction finished
PM_MRK_VSU_FIN | VSU marked instr finish
PM_ISU_REJ_VSU | VSU ISU reject from either pipe
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 259
Event | Description
--- | ---
PM_MRK_L1_RELOAD_VALID | Marked demand reload
PM_LSU_L1_PREF | HW initiated, include SW streaming forms as well, include SW streams as a separate event
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DCACHE_RELOAD_INTV | Combined Intervention event
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 260
Event | Description
--- | ---
PM_SYNC_MRK_L2HIT | Marked L2 Hits that can throw a synchronous interrupt
PM_MRK_L2_RC_DISP | Marked Instruction RC dispatched in L2
PM_MRK_L2_RC_DONE | Marked RC done
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 261
Event | Description
--- | ---
PM_SYNC_MRK_PROBE_NOP | Marked probeNops which can cause synchronous interrupts
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_BACK_BR_CMPL | Marked branch instruction completed with a target address less than current instruction address
PM_PROBE_NOP_DISP | ProbeNops dispatched
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 262
Event | Description
--- | ---
PM_SYNC_MRK_FX_DIVIDE | Marked fixed point divide that can cause a synchronous interrupt
PM_MRK_FXU_FIN | FXU marked instruction finish
PM_ISU_REF_FX0 | FX0 ISU reject
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 263
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DERAT_MISS_64K | Marked Data ERAT Miss (Data TLB Access) page size 64K
PM_MRK_DERAT_MISS_16M | Marked Data ERAT Miss (Data TLB Access) page size 16M
PM_MRK_DERAT_MISS_16G | Marked Data ERAT Miss (Data TLB Access) page size 16G
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 264
Event | Description
--- | ---
PM_MRK_DERAT_MISS_4K | Marked Data ERAT Miss (Data TLB Access) page size 4K
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DERAT_MISS | Erat Miss (TLB Access) All page sizes
PM_MRK_DTLB_MISS_16M | Marked Data TLB Miss page size 16M
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 265
Event | Description
--- | ---
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_MRK_DTLB_MISS_4K | Marked Data TLB Miss page size 4k
PM_MRK_DTLB_MISS_64K | Marked Data TLB Miss page size 64K
PM_MRK_DTLB_MISS | Marked dtlb miss
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

### Group 266
Event | Description
--- | ---
PM_MRK_DTLB_MISS_16G | Marked Data TLB Miss page size 16G
PM_MRK_DTLB_MISS_4K | Marked Data TLB Miss page size 4k
PM_DTLB_MISS | Data PTEG reload
PM_INST_CMPL | Number of PowerPC Instructions that completed. PPC Instructions Finished (completed).
PM_RUN_INST_CMPL | Run instructions
PM_RUN_CYC | Run cycles

Courtesy of [IBM Knowledge Center](https://www.ibm.com/support/knowledgecenter/SSFK5S_2.2.0/com.ibm.cluster.pedev.v2r2.pedev100.doc/bl7ug_power8metrics.htm).

