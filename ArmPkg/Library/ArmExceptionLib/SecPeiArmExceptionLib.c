/* @file
*  Main file supporting the SEC Phase for Versatile Express
*
*  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
*  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
*  Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>
#include <Library/CpuExceptionHandlerLib.h>

#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DefaultExceptionHandlerLib.h>

VOID
ExceptionHandlersStart(
  VOID
  );

// these globals are provided by the architecture specific source (Arm or AArch64)
extern UINTN                    gMaxExceptionNumber;


/**
Initializes all CPU exceptions entries and provides the default exception handlers.

Caller should try to get an array of interrupt and/or exception vectors that are in use and need to
persist by EFI_VECTOR_HANDOFF_INFO defined in PI 1.3 specification.
If caller cannot get reserved vector list or it does not exists, set VectorInfo to NULL.
If VectorInfo is not NULL, the exception vectors will be initialized per vector attribute accordingly.

@param[in]  VectorInfo    Pointer to reserved vector list.

@retval EFI_SUCCESS           CPU Exception Entries have been successfully initialized
with default exception handlers.
@retval EFI_INVALID_PARAMETER VectorInfo includes the invalid content if VectorInfo is not NULL.
@retval EFI_UNSUPPORTED       This function is not supported.

**/
EFI_STATUS
EFIAPI
InitializeCpuExceptionHandlers(
  IN EFI_VECTOR_HANDOFF_INFO       *VectorInfo OPTIONAL
  )
{
  // The AArch64 Vector table must be 2k-byte aligned - if this assertion fails ensure 'Align=4K'
  // is defined into your FDF for this module.
  ASSERT(((UINTN)ExceptionHandlersStart & ARM_VECTOR_TABLE_ALIGNMENT) == 0);

  // We do not copy the Exception Table at PcdGet32(PcdCpuVectorBaseAddress). We just set Vector
  // Base Address to point into CpuDxe code.
  ArmWriteVBar((UINTN)ExceptionHandlersStart);

  return RETURN_SUCCESS;
}

/**
Initializes all CPU interrupt/exceptions entries and provides the default interrupt/exception handlers.

Caller should try to get an array of interrupt and/or exception vectors that are in use and need to
persist by EFI_VECTOR_HANDOFF_INFO defined in PI 1.3 specification.
If caller cannot get reserved vector list or it does not exists, set VectorInfo to NULL.
If VectorInfo is not NULL, the exception vectors will be initialized per vector attribute accordingly.

@param[in]  VectorInfo    Pointer to reserved vector list.

@retval EFI_SUCCESS           All CPU interrupt/exception entries have been successfully initialized
with default interrupt/exception handlers.
@retval EFI_INVALID_PARAMETER VectorInfo includes the invalid content if VectorInfo is not NULL.
@retval EFI_UNSUPPORTED       This function is not supported.

**/
EFI_STATUS
EFIAPI
InitializeCpuInterruptHandlers(
IN EFI_VECTOR_HANDOFF_INFO       *VectorInfo OPTIONAL
)
{
  // not needed in SEC/PEI
  return EFI_UNSUPPORTED;
}

/**
Registers a function to be called from the processor exception handler. (On ARM/AArch64 this only
provides exception handlers, not interrupt handling which is provided through the Hardware Interrupt
Protocol.)

This function registers and enables the handler specified by ExceptionHandler for a processor
interrupt or exception type specified by ExceptionType. If ExceptionHandler is NULL, then the
handler for the processor interrupt or exception type specified by ExceptionType is uninstalled.
The installed handler is called once for each processor interrupt or exception.
NOTE: This function should be invoked after InitializeCpuExceptionHandlers() or
InitializeCpuInterruptHandlers() invoked, otherwise EFI_UNSUPPORTED returned.

@param[in]  ExceptionType     Defines which interrupt or exception to hook.
@param[in]  ExceptionHandler  A pointer to a function of type EFI_CPU_INTERRUPT_HANDLER that is called
when a processor interrupt occurs. If this parameter is NULL, then the handler
will be uninstalled.

@retval EFI_SUCCESS           The handler for the processor interrupt was successfully installed or uninstalled.
@retval EFI_ALREADY_STARTED   ExceptionHandler is not NULL, and a handler for ExceptionType was
previously installed.
@retval EFI_INVALID_PARAMETER ExceptionHandler is NULL, and a handler for ExceptionType was not
previously installed.
@retval EFI_UNSUPPORTED       The interrupt specified by ExceptionType is not supported,
or this function is not supported.
**/
RETURN_STATUS
RegisterCpuInterruptHandler(
  IN EFI_EXCEPTION_TYPE             ExceptionType,
  IN EFI_CPU_INTERRUPT_HANDLER      ExceptionHandler
  ) {
  // not supported in SEC/PEI
  return EFI_UNSUPPORTED;
}

VOID
EFIAPI
CommonCExceptionHandler(
  IN     EFI_EXCEPTION_TYPE           ExceptionType,
  IN OUT EFI_SYSTEM_CONTEXT           SystemContext
  )
{
  if (ExceptionType > gMaxExceptionNumber) {
    DEBUG((EFI_D_ERROR, "Unknown exception type %d\n", ExceptionType));
    ASSERT(FALSE);
  }

  DefaultExceptionHandler(ExceptionType, SystemContext);
}
