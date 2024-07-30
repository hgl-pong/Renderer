@echo off
setlocal enabledelayedexpansion


:: 设置输入和输出目录
set INPUT_DIR=glsl
set OUTPUT_DIR=spv

:: 创建输出目录（如果不存在）
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

:: 遍历所有的.vert, .frag, .comp, .geom, .tesc, .tese文件
for %%f in (%INPUT_DIR%\*.vert %INPUT_DIR%\*.frag %INPUT_DIR%\*.comp %INPUT_DIR%\*.geom %INPUT_DIR%\*.tesc %INPUT_DIR%\*.tese) do (
    echo Compiling %%~nxf...
    glslangValidator.exe -V "%%f" -o "%OUTPUT_DIR%\%%~nf.spv"
    if !errorlevel! neq 0 (
        echo Error compiling %%~nxf
    ) else (
        echo Successfully compiled %%~nxf
    )
)

echo.
echo Compilation complete.
pause
