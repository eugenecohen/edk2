//
//  Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//

#include <AutoGen.h>
#include <AsmMacroIoLibV8.h>
#include <Chipset/AArch64.h>
#include <IndustryStandard/ArmStdSmc.h>

.text
.align 3

GCC_ASM_EXPORT(SyncExceptionHookFunction)

ASM_PFX(RunImageValue):
  .8byte ARM_SMC_ID_ARM_RUN_IMAGE_AARCH64

// This synchronous exception hook looks for a special SMC call to invoke
// a new resident EL3 image (the PI SMM exception handler typically).  It
// uses a saved system context for GPRs pointed to by x0 defined in the
// ARM Exception Handler library.
.align 3
ASM_PFX(SyncExceptionHookFunction):

  // confirm this is an SMC call
  mrs      x1, esr_el3              // EL3 Exception syndrome register 32bit
  lsr      x1, x1, #ESR_EC_SHIFT    // extract Exception Class field
  cmp      x1, #ESR_EC_SMC_AARCH64  // check for SMC originating from AArch64 mode
  bne      NotSmc

  // see if this is a RUN_IMAGE invocation by checking SMC function identifier (x0)
  ldr      x1, [x0, #0x00] // X0
  ldr      x2, RunImageValue
  cmp      x1, x2
  bne      NotSmc

  // restore the GPR context and jump
  ldr      x30, [x0, #0xf0]
  ldr      x29, [x0, #0xe8]
  ldr      x28, [x0, #0xe0]
  ldr      x27, [x0, #0xd8]
  ldr      x26, [x0, #0xd0]
  ldr      x25, [x0, #0xc8]
  ldr      x24, [x0, #0xc0]
  ldr      x23, [x0, #0xb8]
  ldr      x22, [x0, #0xb0]
  ldr      x21, [x0, #0xa8]
  ldr      x20, [x0, #0xa0]
  ldr      x19, [x0, #0x98]
  ldr      x18, [x0, #0x90]
  ldr      x17, [x0, #0x88]
  ldr      x16, [x0, #0x80]
  ldr      x15, [x0, #0x78]
  ldr      x14, [x0, #0x70]
  ldr      x13, [x0, #0x68]
  ldr      x12, [x0, #0x60]
  ldr      x11, [x0, #0x58]
  ldr      x10, [x0, #0x50]
  ldr       x9, [x0, #0x48]
  ldr       x8, [x0, #0x40]
  ldr       x7, [x0, #0x38]
  ldr       x6, [x0, #0x30]
  ldr       x5, [x0, #0x28]
  ldr       x4, [x0, #0x20]
  ldr       x3, [x0, #0x18]
  ldr       x2, [x0, #0x10]
  ldr       x1, [x0, #0x08]
  ldr       x0, [x0, #0x00]

  // x1: Address to Jump To
  br       x1

  // we will never return
  
NotSmc:
  ldr      x1, [sp], #16
  ret
