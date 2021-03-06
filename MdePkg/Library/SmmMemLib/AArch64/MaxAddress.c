/** @file
  Instance of SMM memory check library.

  SMM memory check library library implementation. This library consumes SMM_ACCESS2_PROTOCOL
  to get SMRAM information. In order to use this library instance, the platform should produce
  all SMRAM range via SMM_ACCESS2_PROTOCOL, including the range for firmware (like SMM Core
  and SMM driver) and/or specific dedicated hardware.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2015, Hewlett-Packard Company. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <PiSmm.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

//
// Maximum support address used to check input buffer
//
extern EFI_PHYSICAL_ADDRESS  mSmmMemLibInternalMaximumSupportAddress;

/**
  Calculate and save the maximum support address.

**/
VOID
SmmMemLibInternalCalculateMaximumSupportAddress (
  VOID
  )
{
  UINT8        PhysicalAddressBits;

  // we need to find an intelligent way to get this
  PhysicalAddressBits = 48;
  
  //
  // Save the maximum support address in one global variable  
  //
  mSmmMemLibInternalMaximumSupportAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)(LShiftU64 (1, PhysicalAddressBits) - 1);
  DEBUG ((EFI_D_INFO, "mSmmMemLibInternalMaximumSupportAddress = 0x%lx\n", mSmmMemLibInternalMaximumSupportAddress));
}
