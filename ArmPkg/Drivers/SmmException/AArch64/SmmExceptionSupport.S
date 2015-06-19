//
// Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.<BR>
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

#include <AutoGen.h>
#include <AsmMacroIoLibV8.h>

GCC_ASM_EXPORT(_ModuleEntryPoint)
GCC_ASM_EXPORT(SmmCoreEntryPoint)

GCC_ASM_IMPORT(InitializeExceptionHandler)
GCC_ASM_IMPORT(SyncExceptionHandler)
GCC_ASM_IMPORT(gExceptionHandlers)

.text
.align 11


//
// This is the storage location for the address of the SMM Core Entry Point
// pass during initialization.
//
.align 3
ASM_PFX(SmmCoreEntryPoint):
  .8byte 0

//
// EL3 Exception Handler Initialization
// Arguments:
// X0: SMC funciton identifier for Run Image
// X1: This function's address
// X2: EL3 Stack Pointer
// X3: SMM Core Entry Point
//
.align 3
ASM_PFX(_ModuleEntryPoint):

// Set stack pointer
  mov       sp, x2

// Record SMM entry point for forwarding future SMIs
  adr       x1, ASM_PFX(SmmCoreEntryPoint)
  str       x3, [x1]

// register the custom Synchronous Exception Handler (changes x1 and x2, this is okay)
  adr       x1, gExceptionHandlers
  add       x1, x1, #0 // #define EXCEPT_AARCH64_SYNCHRONOUS_EXCEPTIONS    0
  adr       x2, ASM_PFX(SyncExceptionHandler)
  str       x2, [x1]

// This initialization is called bare from the SEC EL3 SMC vector so jump right in to the
// synchronous exception handler so we return correctly
b           SynchronousExceptionEntry

