set PATH=%PATH%;C:\MinGW\bin
set GCC49_AARCH64_PREFIX=C:\gcc\bin\
call ..\..\edksetup.bat
set STARTTIME=%TIME%
build -a AARCH64 -b DEBUG -t GCC49 -p ArmPlatformPkg/ArmVExpressPkg/ArmVExpress-FVP-AArch64.dsc -D EDK2_ARMVE_STANDALONE=1 -D ARM_FVP_RUN_NORFLASH=1
echo   end: %TIME%
echo start: %STARTTIME%

