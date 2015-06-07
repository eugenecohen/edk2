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
DoSecureConfig (
  IN  EFI_SMM_ACCESS2_PROTOCOL  *SmmAccess
  );

VOID
SmmAccessInstalledNotification(
  IN EFI_EVENT   Event,
  IN VOID        *Context
  );

EFI_STATUS
LoadSmmExceptionModule (
  IN  EFI_SMRAM_DESCRIPTOR            *SmramRange,
  OUT PHYSICAL_ADDRESS                *EntryPoint,
  OUT EFI_SMM_RESERVED_SMRAM_REGION   *ReservedRegion
  );
  
EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  );

STATIC VOID                             *mSmmAccessProtocolNotificationToken = NULL;
STATIC EFI_EVENT                        mSmmAccessProtocolNotificationEvent = (EFI_EVENT)NULL;
STATIC EFI_SMM_ACCESS2_PROTOCOL         *mSmmAccess = (EFI_SMM_ACCESS2_PROTOCOL *)NULL;

STATIC EFI_SMM_ENTRY_POINT              RegisteredEntryPoint = (EFI_SMM_ENTRY_POINT)NULL;

STATIC EFI_SMM_RESERVED_SMRAM_REGION    mSmramReservedRegions[3];

STATIC EFI_HANDLE                       mSmmConfigHandle = NULL;
STATIC EFI_SMM_CONFIGURATION_PROTOCOL   mSmmConfig = {
  mSmramReservedRegions,
  RegisterSmmEntry
};

STATIC EFI_PHYSICAL_ADDRESS   mExceptionHandlerEntryPoint = 0;
STATIC EFI_PHYSICAL_ADDRESS   mSecureStackPointer = 0;

EFI_STATUS
InitializeSecureConfig(VOID) {

  EFI_STATUS                  Status;

  // get SMM Access
  Status = gBS->LocateProtocol(&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&mSmmAccess);
  if (EFI_ERROR(Status)) {

    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, SmmAccessInstalledNotification, NULL, &mSmmAccessProtocolNotificationEvent);
    ASSERT_EFI_ERROR(Status);

    Status = gBS->RegisterProtocolNotify(&gEfiSmmAccess2ProtocolGuid, mSmmAccessProtocolNotificationEvent, (VOID *)&mSmmAccessProtocolNotificationToken);
  }
  else {
    Status = DoSecureConfig(mSmmAccess);
  }

  return Status;
}

VOID
SmmAccessInstalledNotification (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS        Status;

  Status = gBS->LocateProtocol(&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&mSmmAccess);

  if (!EFI_ERROR(Status)) {
    if (mSmmAccess != NULL) {
      DoSecureConfig(mSmmAccess);
    }
  }
}

EFI_STATUS
DoSecureConfig (
  IN  EFI_SMM_ACCESS2_PROTOCOL  *SmmAccess
  ) {

  EFI_STATUS                  Status;
  UINTN                       SmramMapSize;
  EFI_SMRAM_DESCRIPTOR        *SmramMap = (EFI_SMRAM_DESCRIPTOR *)NULL;
  UINTN                       i;
  EFI_SMRAM_DESCRIPTOR        *SelectedDescriptor = (EFI_SMRAM_DESCRIPTOR *)NULL;

  // get the SMRAM map
  SmramMapSize = 0;
  Status = SmmAccess->GetCapabilities(SmmAccess, &SmramMapSize, (EFI_SMRAM_DESCRIPTOR *)NULL);
  if(Status != EFI_BUFFER_TOO_SMALL) return Status;
  SmramMap = AllocatePool(SmramMapSize);
  Status = SmmAccess->GetCapabilities(SmmAccess, &SmramMapSize, SmramMap);
  if (EFI_ERROR(Status)) return Status;

  // find the largest entry
  for (i = 0; i < SmramMapSize / sizeof(SmramMap[0]); i++) {
    if (SelectedDescriptor == NULL) {
      SelectedDescriptor = &SmramMap[i];
    }
    else if (SmramMap[i].PhysicalSize > SelectedDescriptor->PhysicalSize) {
      SelectedDescriptor = &SmramMap[i];
    }
  }
  if (SelectedDescriptor == NULL) {
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  // SMRAM Memory Map
  // [top]
  // SMM Exception Handler (SMRAM reservation #0)
  // Stack, growing downward ((SMRAM reservation #1)
  // <Free to Allocate by SMM Core>
  // [bottom]

  // ensure we have at least 32KB
  if ((SelectedDescriptor->PhysicalSize) < (32 * 1024)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // load the SMM exception handler module, getting the entry point and reserved SMRAM range (entry 0)
  Status = LoadSmmExceptionModule(SelectedDescriptor, &mExceptionHandlerEntryPoint, &mSmramReservedRegions[0]);
  if (EFI_ERROR(Status)) goto ErrorExit;

  // carve out a page for stack below the exception handler
  mSmramReservedRegions[1].SmramReservedSize = EFI_PAGE_SIZE;
  mSmramReservedRegions[1].SmramReservedStart = mSmramReservedRegions[0].SmramReservedStart - mSmramReservedRegions[1].SmramReservedSize;
  mSecureStackPointer = mSmramReservedRegions[1].SmramReservedStart + mSmramReservedRegions[1].SmramReservedSize - 0x10;

  // append the reserved SMRAM range temrinator
  mSmramReservedRegions[2].SmramReservedStart = 0;
  mSmramReservedRegions[2].SmramReservedSize = 0;




  // todo: update exception handlers in ExceptionSupport.S to handle EL3 properly





  Status = gBS->InstallMultipleProtocolInterfaces(
    &mSmmConfigHandle,
    &gEfiSmmConfigurationProtocolGuid, &mSmmConfig,
    NULL
    );

ErrorExit:
  FreePool(SmramMap);
  return Status;
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
  RunImageArgs.Arg1 = (UINTN)mExceptionHandlerEntryPoint;
  RunImageArgs.Arg2 = mSecureStackPointer;
  RunImageArgs.Arg3 = (UINTN)RegisteredEntryPoint;

  ArmCallSmc(&RunImageArgs);

  // null out the entry point since it's been started
  RegisteredEntryPoint = (EFI_SMM_ENTRY_POINT)NULL;

  return EFI_SUCCESS;
}

EFI_GUID gSmmExceptionModuleGuid = { 0x5a708c65, 0x3f08, 0x4b78, { 0x85, 0xfa, 0x2d, 0x3e, 0x14, 0x46, 0x89, 0x83 } };

EFI_STATUS
LoadSmmExceptionModule(
  IN  EFI_SMRAM_DESCRIPTOR            *SmramRange,
  OUT PHYSICAL_ADDRESS                *EntryPoint,
  OUT EFI_SMM_RESERVED_SMRAM_REGION   *ReservedRegion
  )
{
  EFI_STATUS                    Status;
  VOID                          *FvSectionBuffer;
  UINTN                         FvSectionSize;
  PE_COFF_LOADER_IMAGE_CONTEXT  ImageContext;
  UINTN                         PageCount;
  EFI_PHYSICAL_ADDRESS          DestinationBuffer;

  //
  // Find the SMM Exception module's PE32 section
  //  
  Status = GetSectionFromAnyFv(
    &gSmmExceptionModuleGuid,
    EFI_SECTION_PE32,
    0,
    &FvSectionBuffer,
    &FvSectionSize
    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Initilize ImageContext
  //
  ImageContext.Handle = FvSectionBuffer;
  ImageContext.ImageRead = PeCoffLoaderImageReadFromMemory;

  //
  // Get information about the image being loaded
  //
  Status = PeCoffLoaderGetImageInfo(&ImageContext);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Allocate memory for the image being loaded from the EFI_SRAM_DESCRIPTOR 
  // specified by SmramRange
  //
  PageCount = (UINTN)EFI_SIZE_TO_PAGES((UINTN)ImageContext.ImageSize + ImageContext.SectionAlignment);

  ASSERT((SmramRange->PhysicalSize & EFI_PAGE_MASK) == 0);
  ASSERT(SmramRange->PhysicalSize > EFI_PAGES_TO_SIZE(PageCount));

  DestinationBuffer = SmramRange->CpuStart + SmramRange->PhysicalSize - EFI_PAGES_TO_SIZE(PageCount);

  //
  // Align buffer on section boundry
  //
  ImageContext.ImageAddress = DestinationBuffer;

  ImageContext.ImageAddress += ImageContext.SectionAlignment - 1;
  ImageContext.ImageAddress &= ~((EFI_PHYSICAL_ADDRESS)(ImageContext.SectionAlignment - 1));

  ReservedRegion->SmramReservedStart = DestinationBuffer;
  ReservedRegion->SmramReservedSize = EFI_PAGES_TO_SIZE(PageCount);

  //
  // Print debug message showing SMM Core load address.
  //
  DEBUG((DEBUG_INFO, "Loading SMM Exception handler at SMRAM address %p\n", (VOID *)(UINTN)ImageContext.ImageAddress));

  //
  // Load the image to our new buffer
  //
  Status = PeCoffLoaderLoadImage(&ImageContext);
  if (!EFI_ERROR(Status)) {
    //
    // Relocate the image in our new buffer
    //
    Status = PeCoffLoaderRelocateImage(&ImageContext);
    if (!EFI_ERROR(Status)) {
      //
      // Flush the instruction cache so the image data are written before we execute it
      //
      InvalidateInstructionCacheRange((VOID *)(UINTN)ImageContext.ImageAddress, (UINTN)ImageContext.ImageSize);

      *EntryPoint = ImageContext.EntryPoint;
      Status = EFI_SUCCESS;
    }
  }

  //
  // Always free memory allocted by GetSectionFromAnyFv ()
  //
  FreePool(FvSectionBuffer);

  return Status;
}
