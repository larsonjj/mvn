@echo OFF
pushd ..
IF NOT EXIST build mkdir build
IF NOT EXIST build\win mkdir build\win

pushd build\win
cmake ..\.. -G Ninja
IF %ERRORLEVEL% NEQ 0 echo "Failed to run emcmake" && exit /b
timeout /t 2 /nobreak > NUL
cmake --build . --config Release --parallel
IF %ERRORLEVEL% NEQ 0 echo "Failed to build" && exit /b

popd
popd
