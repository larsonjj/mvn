@echo OFF
pushd ..
IF NOT EXIST build mkdir build
IF NOT EXIST build\web mkdir build\web

pushd build\web
emcmake cmake ..\..
IF %ERRORLEVEL% NEQ 0 echo "Failed to run emcmake" && exit /b
timeout /t 2 /nobreak > NUL
cmake --build . --config Release --parallel
IF %ERRORLEVEL% NEQ 0 echo "Failed to build" && exit /b

popd
popd
