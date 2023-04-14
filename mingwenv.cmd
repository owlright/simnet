set "CURRENT_DIR=%CD%"
for %%I in ("%CURRENT_DIR%") do set "PARENT_DIR=%%~dpI"

call %PARENT_DIR%omnetpp-6.0.1\tools\win32.x86_64\msys2_shell.cmd -mingw64 -defterm -here -no-start %*