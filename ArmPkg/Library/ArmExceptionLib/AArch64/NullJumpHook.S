//
//  Copyright (c) 2015 Hewlett-Packard Company. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//

#include <AutoGen.h>
#include <AsmMacroIoLibV8.h>

.text
.align 3

GCC_ASM_EXPORT(SyncExceptionHookFunction)

.align 3
ASM_PFX(SyncExceptionHookFunction):
  ret
