/** @file
  This library is to abstract TPM2 register accesses so that a common
  interface can be used for multiple underlying busses such as TPM,
  SPI, or I2C access.

Copyright (c) 2018 HP Development Company, L.P.
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>

#include <Library/TpmIoLib.h>
#include <Library/IoLib.h>



/**
  Reads an 8-bit TPM register.

  Reads the 8-bit TPM register specified by Address. The 8-bit read value is
  returned. This function must guarantee that all TPM read and write
  operations are serialized.

  If 8-bit TPM register operations are not supported, then ASSERT().

  @param  Address The TPM register to read.

  @return The value read.

**/
UINT8
EFIAPI
TpmRead8 (
  IN      UINTN                     Address
  )
{
  return MmioRead8 (Address);
}

/**
  Writes an 8-bit TPM register.

  Writes the 8-bit TPM register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all TPM read
  and write operations are serialized.

  If 8-bit TPM register operations are not supported, then ASSERT().

  @param  Address The TPM register to write.
  @param  Value   The value to write to the TPM register.

  @return Value.

**/
UINT8
EFIAPI
TpmWrite8 (
  IN      UINTN                     Address,
  IN      UINT8                     Value
  )
{
  return MmioWrite8 (Address, Value);
}


/**
  Reads a 16-bit TPM register.

  Reads the 16-bit TPM register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all TPM read and write
  operations are serialized.

  If 16-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The TPM register to read.

  @return The value read.

**/
UINT16
EFIAPI
TpmRead16 (
  IN      UINTN                     Address
  )
{
  return MmioRead16 (Address);
}


/**
  Writes a 16-bit TPM register.

  Writes the 16-bit TPM register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all TPM read
  and write operations are serialized.

  If 16-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The TPM register to write.
  @param  Value   The value to write to the TPM register.

  @return Value.

**/
UINT16
EFIAPI
TpmWrite16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  )
{
  return MmioWrite16 (Address, Value);
}

/**
  Reads a 32-bit TPM register.

  Reads the 32-bit TPM register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all TPM read and write
  operations are serialized.

  If 32-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The TPM register to read.

  @return The value read.

**/
UINT32
EFIAPI
TpmRead32 (
  IN      UINTN                     Address
  )
{
  return MmioRead32 (Address);
}

/**
  Writes a 32-bit TPM register.

  Writes the 32-bit TPM register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all TPM read
  and write operations are serialized.

  If 32-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The TPM register to write.
  @param  Value   The value to write to the TPM register.

  @return Value.

**/
UINT32
EFIAPI
TpmWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  )
{
  return MmioWrite32 (Address, Value);
}


/**
  Reads a 64-bit TPM register.

  Reads the 64-bit TPM register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all TPM read and write
  operations are serialized.

  If 64-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The TPM register to read.

  @return The value read.

**/
UINT64
EFIAPI
TpmRead64 (
  IN      UINTN                     Address
  )
{
  return MmioRead64 (Address);
}

/**
  Writes a 64-bit TPM register.

  Writes the 64-bit TPM register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all TPM read
  and write operations are serialized.

  If 64-bit TPM register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The TPM register to write.
  @param  Value   The value to write to the TPM register.

**/
UINT64
EFIAPI
TpmWrite64 (
  IN      UINTN                     Address,
  IN      UINT64                    Value
  )
{
  return MmioWrite64 (Address, Value);
}
