/** @file

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
#include <Library/CpuExceptionHandlerLib.h>
#include <Library/DefaultExceptionHandlerLib.h>

#include <Pi/PiSmmCis.h>
#include <Pi/PiDxeCis.h> // for EFI_AP_PROCEDURE
#include <Protocol/DebugSupport.h> // for exception types
#include <IndustryStandard/ArmStdSmc.h>

VOID
InitalizeSmmException(VOID);

EFI_STATUS
EFIAPI
ArmSmmStartupThisAp(
  IN EFI_AP_PROCEDURE  Procedure,
  IN UINTN             CpuNumber,
  IN OUT VOID          *ProcArguments OPTIONAL
  );

BOOLEAN
RouteExceptionClassToSmm(
  IN  UINTN   ExceptionClass
  );

// global variables implemented in SmmException.S
extern EFI_SMM_ENTRY_POINT SmmCoreEntryPoint;

BOOLEAN mSmmEnvInitialized = FALSE;
UINTN gNumberOfCpus;

VOID
EFIAPI
SyncExceptionHandler(
IN     EFI_EXCEPTION_TYPE           ExceptionType,
IN OUT EFI_SYSTEM_CONTEXT           SystemContext
)
{
  UINTN Ec;

  // extract the Exception Class from the Exception Syndrome Register
  Ec = (SystemContext.SystemContextAArch64->ESR & ESR_EC_MASK) >> ESR_EC_SHIFT;

  //
  // Synchronous Exceptions are categorized into two classes:
  //   1. Fatal exceptions that are forwarded to the DefaultExceptionHandler library
  //   2. Events that should be forwarded to the SMM Foundation like SWSMI (SMC) and Interrupts
  // 
  // We will treat all trapped accesses and SVC/HVC/SMCs and forward them to the SMM foundation.
  // Other sources (faults and aborts) will go to the DefaultExceptionHandler.
  //

  if (RouteExceptionClassToSmm(Ec)) {
    EFI_SMM_ENTRY_CONTEXT   EntryContext;

    // todo: update for multiple processors
    UINTN                       SaveStateSize = sizeof(EFI_SYSTEM_CONTEXT_AARCH64);
    VOID                        *SaveStateList[1];

    SaveStateList[0] = SystemContext.SystemContextAArch64;

    // first invocation, do intialization
    if (!mSmmEnvInitialized) {
    
      InitalizeSmmException();
    
    }

    // prepare the entry context
    EntryContext.SmmStartupThisAp = ArmSmmStartupThisAp;
    EntryContext.CurrentlyExecutingCpu = 0; // TODO: make this real
    EntryContext.NumberOfCpus = gNumberOfCpus;
    EntryContext.CpuSaveStateSize = &SaveStateSize;
    EntryContext.CpuSaveState = SaveStateList;

    // invoke the SMM Core Entry Point
    SmmCoreEntryPoint(&EntryContext);

  } else {

    // all other exceptions go to the DefaultExceptionHandler
    DefaultExceptionHandler(ExceptionType, SystemContext);

  }
}

VOID
InitalizeSmmException(VOID) {
  
  // take over exception handlers at this privilege level
  InitializeCpuExceptionHandlers(NULL);

  gNumberOfCpus = 1; // TODO: make this real

  mSmmEnvInitialized = TRUE;

  // do other first-time setup stuff like configuring page tables, etc
  // SCTLR_EL3
  // SCR_EL3

  // TODO
}

BOOLEAN
RouteExceptionClassToSmm(
  IN  UINTN   ExceptionClass
  ) {

  BOOLEAN RouteToSmm;
  switch (ExceptionClass) {

    // trapped accesses
    case ESR_EC_WFI_WFE:
    case ESR_EC_CP15_MCR_MRC:
    case ESR_EC_CP15_MCRR_MRRC:
    case ESR_EC_CP14_MCR_MRC:
    case ESR_EC_CP14_LDC_STC:
    case ESR_EC_FP_SIMD_ACCESS:
    case ESR_EC_CP10_MCR_MRC:
    case ESR_EC_CP14_MRRC:
    case ESR_EC_MSR_MRS:
    // system calls
    case ESR_EC_SVC_AARCH32:
    case ESR_EC_HVC_AARCH32:
    case ESR_EC_SMC_AARCH32:
    case ESR_EC_SVC_AARCH64:
    case ESR_EC_HVC_AARCH64:
    case ESR_EC_SMC_AARCH64:
      RouteToSmm = TRUE;
      break;
    
    // faults and aborts
    default:
      RouteToSmm = FALSE;
      break;
  }

  return RouteToSmm;
}

EFI_STATUS
EFIAPI
ArmSmmStartupThisAp(
  IN EFI_AP_PROCEDURE  Procedure,
  IN UINTN             CpuNumber,
  IN OUT VOID          *ProcArguments OPTIONAL
  ) {
  
  // TODO: implement this
  return EFI_UNSUPPORTED;
}
