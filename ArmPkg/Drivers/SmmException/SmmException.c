/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Portions Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DefaultExceptionHandlerLib.h>

#include <Pi/PiSmmCis.h>
#include <Protocol/DebugSupport.h> // for exception types
//#include <IndustryStandard/ArmStdSmc.h>

VOID
EFIAPI
CommonCExceptionHandler(
  IN     EFI_EXCEPTION_TYPE           ExceptionType,
  IN OUT EFI_SYSTEM_CONTEXT           SystemContext
  );

VOID
EFIAPI
SyncExceptionHandler(
  IN     EFI_EXCEPTION_TYPE           ExceptionType,
  IN OUT EFI_SYSTEM_CONTEXT           SystemContext
  );

VOID
  ExceptionHandlersStart(
  VOID
  );

// global variables implemented in SmmException.S
extern UINTN CommonExceptionEntry;
extern EFI_SMM_ENTRY_POINT SmmCoreEntryPoint;

EFI_EXCEPTION_CALLBACK  gExceptionHandlers[MAX_AARCH64_EXCEPTION + 1];

VOID
EFIAPI
InitializeExceptionHandler(
IN     EFI_EXCEPTION_TYPE           ExceptionType,
IN OUT EFI_SYSTEM_CONTEXT           SystemContext
)
{
  DEBUG((EFI_D_INFO, "SmmException: InitializeExceptionHandler"));
  DEBUG((EFI_D_INFO, "SMM Core entry point at 0x%x", (UINTN)SmmCoreEntryPoint));

  ZeroMem(gExceptionHandlers, sizeof(*gExceptionHandlers));

  // update exception handler function address for future exception handling
  CommonExceptionEntry = (UINTN)CommonCExceptionHandler;

  // write the EL3 VBAR with the SMM vector table
  ArmWriteVBar((UINTN)ExceptionHandlersStart);

  // register a synchronous exception handler
  gExceptionHandlers[EXCEPT_AARCH64_SYNCHRONOUS_EXCEPTIONS] = SyncExceptionHandler;
}

VOID
EFIAPI
SyncExceptionHandler(
IN     EFI_EXCEPTION_TYPE           ExceptionType,
IN OUT EFI_SYSTEM_CONTEXT           SystemContext
)
{
  // TODO
  // demultiplex synchronous exceptions
  // call SMC handler


  // for now route everything to default handler
  DefaultExceptionHandler(ExceptionType, SystemContext);
}

