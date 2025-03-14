@echo off
echo 正在构建任务栏时钟应用程序...

:: 创建构建目录
if not exist build mkdir build
cd build
 
rm -r *

:: 运行CMake配置
cmake .. -A Win32

:: 构建项目
cmake --build . --config Release

:: 复制可执行文件到根目录
copy bin\Release\TaskbarClock.exe ..\日历.exe

cd ..
echo 构建完成！TaskbarClock.exe 已生成。 