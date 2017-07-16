/*
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* Warning! verify consistency of get_frame_off() function that uses this:
 */
#define MEMARG_OFFSET 8

extern const int scratch_regs[];

typedef enum {
  NO_REG = -1,
  IR_X0 = 1,    // param/result 1
  IR_X1,        // param/result 2
  IR_X2,        // param/result 3
  IR_X3,        // param/result 4
  IR_X4,        // param/result 5
  IR_X5,        // param/result 6
  IR_X6,        // param/result 7
  IR_X7,        // param/result 8
  IR_X8,        // indirect result location
  IR_X9,        // tmp 1
  IR_X10,       // tmp 2
  IR_X11,       // tmp 3
  IR_X12,       // tmp 4
  IR_X13,       // tmp 5
  IR_X14,       // tmp 6
  IR_X15,       // tmp 7
  IR_X16,       // IP0
  IR_X17,       // IP1
  IR_X18,       // context
  IR_X19,       // callee-saved 1
  IR_X20,       // callee-saved 2
  IR_X21,       // callee-saved 3
  IR_X22,       // callee-saved 4
  IR_X23,       // callee-saved 5
  IR_X24,       // callee-saved 6
  IR_X25,       // callee-saved 7
  IR_X26,       // callee-saved 8
  IR_X27,       // callee-saved 9
  IR_X28,       // callee-saved 10
  IR_X29,       // frame pointer
  IR_X30,       // link register
  IR_X31        // stack pointer
} IR_REGS;

#define IR_FIRST_CALLEE_SAVE IR_X19
#define IR_LAST_CALLEE_SAVE IR_X28

typedef enum {
  XR_V0 = 1,    // param 1
  XR_V1,        // param 2
  XR_V2,        // param 3
  XR_V3,        // param 4
  XR_V4,        // param 5
  XR_V5,        // param 6
  XR_V6,        // param 7
  XR_V7,        // param 8
  XR_V8,        // callee-saved 1
  XR_V9,        // callee-saved 2
  XR_V10,       // callee-saved 3
  XR_V11,       // callee-saved 4
  XR_V12,       // callee-saved 5
  XR_V13,       // callee-saved 6
  XR_V14,       // callee-saved 7
  XR_V15,       // callee-saved 8
  XR_V16,       // tmp 1
  XR_V17,       // tmp 2
  XR_V18,       // tmp 3
  XR_V19,       // tmp 4
  XR_V20,       // tmp 5
  XR_V21,       // tmp 6
  XR_V22,       // tmp 7
  XR_V23,       // tmp 8
  XR_V24,       // tmp 9
  XR_V25,       // tmp 10
  XR_V26,       // tmp 11
  XR_V27,       // tmp 12
  XR_V28,       // tmp 13
  XR_V29,       // tmp 14
  XR_V30,       // tmp 15
  XR_V31        // tmp 16
} XR_REGS;

#define XR_FIRST_CALLEE_SAVE XR_V8
#define XR_LAST_CALLEE_SAVE XR_V15

#define MAX_N_REGS (IR_X31 + XR_V31)

#define FR_RETVAL XR_V0
#define SP_RETVAL XR_V0
#define DP_RETVAL XR_V0
#define CS_RETVAL XR_V0
#define CD_RETVAL XR_V0
#define IR_RETVAL IR_X0
#define AR_RETVAL IR_X0

/* Machine Register Information -
 *
 * This information is in two pieces:
 * 1.  a structure exists for each machine's register class which defines
 *     the attributes of registers.
 *     These attributes define a register set with the following properties:
 *         1).  a set is just an increasing set of numbers,
 *         2).  scratch registers are allocated in increasing order (towards
 *              the globals),
 *         3).  globals are allocated in decreasing order (towards the
 *              scratch registers).
 *         4).  the scratch set that can be changed by a procedure
 *              [min .. first_global-1]
 *         5).  the scratch set that can be changed by an intrinisic
 *              [min .. intrinsic]
 *
 * 2.  a structure exists for all of the generic register classes which will
 *     map a register type (macros in registers.h) to the appropriate
 *     machine register class.
 */

/*****  Machine Register Table  *****/

typedef struct {
  char min;       /* minimum register # */
  char max;       /* maximum register # */
  char intrinsic; /* last scratch that can be changed by an intrinsic */
  const char first_global; /* first register # that can be global
                      * Note that the globals are allocated in increasing
                      * order (first_global down to last_global).
                      */
  const char end_global;   /* absolute last register # that can be global. */
                           /* the following two really define the working set
                              of registers that can be assigned. */
  char next_global;        /* next global register # */
  char last_global;        /* last register # that can be global. */
  char nused;              /* number of global registers assigned */
  const char class;        /* class or type of register.  code generator needs
                              to know what kind of registers these represent.
                              'i' (integer), 'f' (float stk), 'x' (float xmm) */
} MACH_REG;

/*****  Register Mapping Table  *****/

typedef struct {
  char max;           /* maximum number of registers */
  char nused;         /* number of registers assigned */
  char joined;        /* non-zero value if registers are formed from
                       * multiple machine registers. 1==>next machine
                       * register is used; other values TBD.
                       */
  int rcand;          /* register candidate list */
  MACH_REG *mach_reg; /* pointer to struct of the actual registers */
  INT const_flag;     /* flag controlling assignment of consts */
} REG;                /*  [rtype]  */

/*****  Register Set Information for a block  *****/

typedef struct { /* three -word bit-vector */
  int xr;
} RGSET;

typedef struct {
  RGSET *stg_base;
  int stg_avail;
  int stg_size;
} RGSETB;

#define SCRATCH_REGS { IR_X9, IR_X10, IR_X11, IR_X12, IR_X13, IR_X14, IR_X15 }

#define MR_MAX_XREG_ARGS 8
#define MR_ARG_XREGS { XR_V0, XR_V1, XR_V2, XR_V3, XR_V4, XR_V5, XR_V6, XR_V7 }

#define MR_MAX_IREG_RES 1
#define MR_RES_IREGS { IR_X0 }

#define MR_UNIQ 3
#define MACH_REGS \
  { \
    { IR_X0, IR_X31, IR_X15, IR_X9, IR_X15, IR_X15, IR_X15, 0, 'i' }, \
    { XR_V0, XR_V31, XR_V31, XR_V16, XR_V31, XR_V31, XR_V31, 0, 'f' }, \
    { XR_V0, XR_V31, XR_V31, XR_V16, XR_V31, XR_V31, XR_V31, 0, 'x' } \
  }

#define REGS(mach_regs) \
  { \
    {12, 0, 0, 0, &((mach_regs)[0]), RCF_NONE}, /*  IR  */   \
    {6, 0, 0, 0, &((mach_regs)[1]), RCF_NONE},  /*  SP  */   \
    {6, 0, 0, 0, &((mach_regs)[1]), RCF_NONE},  /*  DP  */   \
    {12, 0, 0, 0, &((mach_regs)[0]), RCF_NONE}, /*  AR  */   \
    {6, 0, 0, 0, &((mach_regs)[0]), RCF_NONE},  /*  KR  */   \
    {0, 0, 0, 0, 0, 0},                         /*  VECT  */ \
    {0, 0, 0, 0, 0, 0},                         /*  QP    */ \
    {6, 0, 0, 0, 0, RCF_NONE},                  /*  CSP   */ \
    {6, 0, 0, 0, 0, RCF_NONE},                  /*  CDP   */ \
    {0, 0, 0, 0, 0, 0},                         /*  CQP   */ \
    {0, 0, 0, 0, 0, 0},                         /*  X87   */ \
    {0, 0, 0, 0, 0, 0},                         /*  CX87  */ \
    /* the following will be mapped over SP and DP above */ \
    {6, 0, 0, 0, &((mach_regs)[2]), RCF_NONE},  /*  SPXM  */ \
    {6, 0, 0, 0, &((mach_regs)[2]), RCF_NONE},  /*  DPXM  */ \
  }

#define MR_RESET_FRGLOBALS(mach_regs) \
  { \
    /* effectively turn off fp global regs. */ \
    (mach_regs)[1].last_global = mach_reg[1].first_global - 1; \
    (mach_regs)[2].last_global = mach_reg[2].first_global - 1; \
  }

extern int mr_arg_xr[MR_MAX_XREG_ARGS + 1]; // 1-based, element 0 never used
extern int mr_res_ir[MR_MAX_IREG_RES + 1]; // 1-based, element 0 never used

#define ARG_IR(i) (scratch_regs[i])
#define ARG_XR(i) (mr_arg_xr[i])
#define RES_IR(i) (mr_res_ir[i])

#define AR(i) IR_RETVAL
#define IR(i) ARG_IR(i)
#define SP(i) ARG_XR(i)
#define DP(i) ARG_XR(i)

#define ISP(i) ((i) + 100) // suspicious
#define IDP(i) ((i) + 100) // suspicious

#define IR_ARET IR_RETVAL

/* NOTE: for hammer, KR regs are just the IR regs.
 */
#define KR_LSH(i) (i)
#define KR_MSH(i) (i)

/* Macro for defining the KR register in which the value of a 64-bit integer
 * function is returned.
 */
#define KR_RETVAL IR_RETVAL

#define GR_THRESHOLD 2

/*****  External Data  Declarations  *****/

extern REG reg[];
extern RGSETB rgsetb;

#define RGSET_XR(i) rgsetb.stg_base[i].xr

/*****  External Function Declarations  *****/

extern int mr_getnext(int rtype);
extern int mr_getreg(int rtype);
extern int mr_get_rgset();
extern void mr_end();
extern void mr_init();
extern void mr_reset_frglobals();
extern void mr_reset(int rtype);
extern void mr_reset_numglobals(int);
