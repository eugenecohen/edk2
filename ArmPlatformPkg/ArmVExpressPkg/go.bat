set ARMLMD_LICENSE_FILE=8224@eda-wwan1.houston.hp.com;8224@eda-wwan2.houston.hp.com;8224@eda-wwan3.houston.hp.com
set MODEL_SHELL_PATH="C:\Program Files (x86)\ARM\FastModelsTools_9.2/bin/model_shell.exe"
set MODEL_PATH="C:\Program Files (x86)\ARM\FastModelsPortfolio_9.2\examples\FVP_Base\Build_Cortex-A57x1-A53x1\Win64-Release-VC2013\cadi_system_Win64-Release-VC2013.dll"
set FLASH_FILE=C:\edk2\Build\ArmVExpress-FVP-AArch64\DEBUG_GCC49\FV\FVP_AARCH64_EFI.fd
set ROM_FILE=C:\edk2\Build\ArmVExpress-FVP-AArch64\DEBUG_GCC49\FV\FVP_AARCH64_EFI_SEC.fd
start /B C:\bios\edk2\HpLj\T32\t32cadi_fvp.cmd
%MODEL_SHELL_PATH% ^
	 --parameter bp.secureflashloader.fname=%ROM_FILE% ^
	 --parameter bp.flashloader0.fname=%FLASH_FILE% ^
	 --parameter bp.tzc_400.diagnostics=4 ^
	-m %MODEL_PATH% --no-ctrl-c -S



@rem --parameter bp.secure_memory=false ^