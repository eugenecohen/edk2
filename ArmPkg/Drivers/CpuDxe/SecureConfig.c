/** @file
*
*  Copyright (c) 2015, Hewlett-Packard Company. All rights reserved.<BR>
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

#include "CpuDxe.h"

EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  );

STATIC EFI_SMM_ENTRY_POINT RegisteredEntryPoint = (EFI_SMM_ENTRY_POINT)NULL;

STATIC EFI_SMM_RESERVED_SMRAM_REGION SmramReservedRegions = {
  // end record (no reserved ranges)
  0,  // SmramReservedStart
  0,  // SmramReservedSize
};

STATIC EFI_HANDLE                       mSmmConfigHandle = NULL;
STATIC EFI_SMM_CONFIGURATION_PROTOCOL   mSmmConfig = {
  &SmramReservedRegions,
  RegisterSmmEntry
};

EFI_STATUS
InitializeSecureConfig(VOID) {

  return gBS->InstallMultipleProtocolInterfaces(
    &mSmmConfigHandle,
    &gEfiSmmConfigurationProtocolGuid, &mSmmConfig,
    NULL
    );
}

EFI_STATUS
EFIAPI
RegisterSmmEntry(
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  )
{
  RegisteredEntryPoint = SmmEntryPoint;
  return EFI_SUCCESS;
}

EFI_STATUS StartSmmCore(VOID)
{
  ARM_SMC_ARGS    RunImageArgs;

  // only issue the SMC to start the SMM core if an entry point has been registered
  if (RegisteredEntryPoint == NULL) return EFI_NOT_FOUND;
    
  // generate an SMC of type RUN_IMAGE_SMC to get EL3 to start the SMM Core
  // we do expect EL3 to return back to us (eret) so we can resume
  ZeroMem(&RunImageArgs, sizeof(RunImageArgs));

  RunImageArgs.Arg0 = (MAX_BIT > 0xFFFFFFFFULL) ? ARM_SMC_ID_ARM_RUN_IMAGE_AARCH64 : ARM_SMC_ID_ARM_RUN_IMAGE_AARCH32;
  RunImageArgs.Arg1 = (UINTN)RegisteredEntryPoint;

  ArmCallSmc(&RunImageArgs);

  // null out the entry point since it's been started
  RegisteredEntryPoint = (EFI_SMM_ENTRY_POINT)NULL;

  return EFI_SUCCESS;
}
