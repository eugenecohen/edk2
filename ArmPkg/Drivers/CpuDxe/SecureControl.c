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

STATIC BOOLEAN                    mSmmCoreStarted = FALSE;
STATIC EFI_HANDLE                 mSmmControlHandle = NULL;
STATIC EFI_SMM_CONTROL2_PROTOCOL  mSmmControl = {
  SmmActivate,
  SmmDeactivate,
  0 // unused since we don't support periodic SMCs
};

EFI_STATUS
InitializeSecureControl(VOID) {

  return gBS->InstallMultipleProtocolInterfaces(
    &mSmmControlHandle,
    &gEfiSmmControl2ProtocolGuid, &mSmmControl,
    NULL
    );
}

/**
Invokes SMI activation from either the preboot or runtime environment.

This function generates an SMI.

@param[in]     This                The EFI_SMM_CONTROL2_PROTOCOL instance.
@param[in,out] CommandPort         The value written to the command port.
@param[in,out] DataPort            The value written to the data port.
@param[in]     Periodic            Optional mechanism to engender a periodic stream.
@param[in]     ActivationInterval  Optional parameter to repeat at this period one
time or, if the Periodic Boolean is set, periodically.

@retval EFI_SUCCESS            The SMI/PMI has been engendered.
@retval EFI_DEVICE_ERROR       The timing is unsupported.
@retval EFI_INVALID_PARAMETER  The activation period is unsupported.
@retval EFI_INVALID_PARAMETER  The last periodic activation has not been cleared.
@retval EFI_NOT_STARTED        The SMM base service has not been initialized.
**/
EFI_STATUS
EFIAPI
SmmActivate (
  IN CONST EFI_SMM_CONTROL2_PROTOCOL  *This,
  IN OUT UINT8                        *CommandPort       OPTIONAL,
  IN OUT UINT8                        *DataPort          OPTIONAL,
  IN BOOLEAN                          Periodic           OPTIONAL,
  IN UINTN                            ActivationInterval OPTIONAL
  ) {
  EFI_STATUS      Status;
  ARM_SMC_ARGS    SwSmiArgs;

  if (Periodic) return EFI_UNSUPPORTED;

  // if we haven't called the smm core entry point, do that first
  if (!mSmmCoreStarted) {
    
    Status = StartSmmCore();
    if (EFI_ERROR(Status)) return Status;

    mSmmCoreStarted = TRUE;
  }

  // generate a Secure Monitor Call to signal a software SMI to the PI SMM environment
  ZeroMem(&SwSmiArgs, sizeof(SwSmiArgs));
  SwSmiArgs.Arg0 = ARM_SMC_ID_SW_SMI;

  ArmCallSmc(&SwSmiArgs);

  return (SwSmiArgs.Arg0 == 0) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/**
Clears any system state that was created in response to the Trigger() call.

This function acknowledges and causes the deassertion of the SMI activation source.

@param[in] This                The EFI_SMM_CONTROL2_PROTOCOL instance.
@param[in] Periodic            Optional parameter to repeat at this period one time

@retval EFI_SUCCESS            The SMI/PMI has been engendered.
@retval EFI_DEVICE_ERROR       The source could not be cleared.
@retval EFI_INVALID_PARAMETER  The service did not support the Periodic input argument.
**/
EFI_STATUS
EFIAPI
SmmDeactivate (
IN CONST EFI_SMM_CONTROL2_PROTOCOL  *This,
IN BOOLEAN                          Periodic OPTIONAL
) {
  return EFI_UNSUPPORTED;
}
