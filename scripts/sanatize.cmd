@echo off
REM --generate-coredump yes --kernel-regex kns=delaunay --launch-skip 6 --check-cache-control yes
::call compute-sanitizer  --log-file sanitize_memcheck.log --tool memcheck  --leak-check full ..\build\bin\Debug\RXMesh_test.exe --gtest_filter=RXMeshDynamic.RandomFlips
call compute-sanitizer  --log-file sanitize_racecheck.log --tool racecheck  --racecheck-report analysis ..\build\bin\Debug\RXMesh_test.exe --gtest_filter=RXMeshDynamic.RandomFlips
::call compute-sanitizer  --log-file sanitize_initcheck.log --tool initcheck --track-unused-memory yes ..\build\bin\Debug\RXMesh_test.exe --gtest_filter=RXMeshDynamic.RandomFlips
REM call compute-sanitizer  --log-file sanitize_synccheck.log --tool synccheck   ..\build\bin\Debug\DelaunayEdgeFlip.exe

::call compute-sanitizer  --log-file sanitize_memcheck.log --tool memcheck  --leak-check full ..\build\bin\Debug\DelaunayEdgeFlip.exe
::call compute-sanitizer  --log-file sanitize_racecheck.log --tool racecheck  --racecheck-report analysis ..\build\bin\Debug\DelaunayEdgeFlip.exe
::call compute-sanitizer  --log-file sanitize_initcheck.log --tool initcheck --track-unused-memory yes ..\build\bin\Debug\DelaunayEdgeFlip.exe