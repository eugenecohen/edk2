/** @file

  Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ArmVExpressInternal.h"

#include <Pi/PiDxeCis.h>
#include <Protocol/SmmAccess2.h>

#include <Library/HobLib.h>
#include <Library/DxeServicesTableLib.h>

// todo: obtain this from a HOB or PCD
#define SECURE_MEMORY_BASE    0x80000000 // FVP low dram
#define SECURE_MEMORY_SIZE    0x02000000 // 32MB

STATIC EFI_HANDLE               mSmmAccessHandle = NULL;
STATIC EFI_SMM_ACCESS2_PROTOCOL mSmmAccess = {

  SecureMemoryOpen,
  SecureMemoryClose,
  SecureMemoryLock,
  SecureMemoryGetCapabilities,
  FALSE,  // LockState
  TRUE    // OpenState

};

/**
* Initialize secure memory access services.  The primary purpose for this
  is to describe the secure memory ranges to the SMM IPL component.
*/
EFI_STATUS
EFIAPI
InitSecureMemoryAccess(VOID) {

  EFI_STATUS  Status;

  // add secure memory to GCD so SMM IPL can mess with cacheability for it
  Status = gDS->AddMemorySpace(
    EfiGcdMemoryTypeSystemMemory,
    SECURE_MEMORY_BASE, SECURE_MEMORY_SIZE,
    EFI_MEMORY_WB | EFI_MEMORY_RUNTIME
    );
  if (EFI_ERROR(Status)) return Status;

  return gBS->InstallMultipleProtocolInterfaces(
    &mSmmAccessHandle,
    &gEfiSmmAccess2ProtocolGuid, &mSmmAccess,
    NULL
    );
}

/**
Opens the SMRAM area to be accessible by a boot-service driver.

This function "opens" SMRAM so that it is visible while not inside of SMM. The function should
return EFI_UNSUPPORTED if the hardware does not support hiding of SMRAM. The function
should return EFI_DEVICE_ERROR if the SMRAM configuration is locked.

@param[in] This           The EFI_SMM_ACCESS2_PROTOCOL instance.

@retval EFI_SUCCESS       The operation was successful.
@retval EFI_UNSUPPORTED   The system does not support opening and closing of SMRAM.
@retval EFI_DEVICE_ERROR  SMRAM cannot be opened, perhaps because it is locked.
**/
EFI_STATUS
EFIAPI
SecureMemoryOpen(
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  ) {
  // SMRAM defaults to open
  return EFI_SUCCESS;
}

/**
Inhibits access to the SMRAM.

This function "closes" SMRAM so that it is not visible while outside of SMM. The function should
return EFI_UNSUPPORTED if the hardware does not support hiding of SMRAM.

@param[in] This           The EFI_SMM_ACCESS2_PROTOCOL instance.

@retval EFI_SUCCESS       The operation was successful.
@retval EFI_UNSUPPORTED   The system does not support opening and closing of SMRAM.
@retval EFI_DEVICE_ERROR  SMRAM cannot be closed.
**/
EFI_STATUS
EFIAPI
SecureMemoryClose(
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  ) {
  // all SMRAM will be closed at Lock, don't do anything now
  return EFI_SUCCESS;
}

/**
Inhibits access to the SMRAM.

This function prohibits access to the SMRAM region.  This function is usually implemented such
that it is a write-once operation.

@param[in] This          The EFI_SMM_ACCESS2_PROTOCOL instance.

@retval EFI_SUCCESS      The device was successfully locked.
@retval EFI_UNSUPPORTED  The system does not support locking of SMRAM.
**/
EFI_STATUS
EFIAPI
SecureMemoryLock(
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  ) {
  // TODO: send SMC to lock down secure memory
  return EFI_SUCCESS;
}

/**
Queries the memory controller for the possible regions that will support SMRAM.

@param[in]     This           The EFI_SMM_ACCESS2_PROTOCOL instance.
@param[in,out] SmramMapSize   A pointer to the size, in bytes, of the SmramMemoryMap buffer.
@param[in,out] SmramMap       A pointer to the buffer in which firmware places the current memory map.

@retval EFI_SUCCESS           The chipset supported the given resource.
@retval EFI_BUFFER_TOO_SMALL  The SmramMap parameter was too small.  The current buffer size
needed to hold the memory map is returned in SmramMapSize.
**/
EFI_STATUS
EFIAPI
SecureMemoryGetCapabilities(
  IN CONST EFI_SMM_ACCESS2_PROTOCOL  *This,
  IN OUT UINTN                       *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  ) {

  if (SmramMapSize == NULL) return EFI_INVALID_PARAMETER;

  if (*SmramMapSize < sizeof(EFI_SMRAM_DESCRIPTOR)) {
    *SmramMapSize = sizeof(EFI_SMRAM_DESCRIPTOR);
    return EFI_BUFFER_TOO_SMALL;
  }

  if (SmramMap == NULL) return EFI_INVALID_PARAMETER;

  // todo: obtain this from a HOB
  SmramMap[0].PhysicalStart = SECURE_MEMORY_BASE;
  SmramMap[0].CpuStart = SmramMap[0].PhysicalStart;
  SmramMap[0].PhysicalSize = SECURE_MEMORY_SIZE;
  SmramMap[0].RegionState = EFI_SMRAM_OPEN;

  return EFI_SUCCESS;
}
