@echo off

rem Apply the patch
git apply --reject --whitespace=fix sdk_patch

rem Remove the specified directories
@REM rd /s /q .\sdk\common\src\drivers\cxpi
@REM rd /s /q .\sdk\common\src\drivers\smif
@REM rd /s /q .\sdk\tviibe1m\hdr\rev_b
@REM rd /s /q .\sdk\tviibe1m\hdr\rev_c
