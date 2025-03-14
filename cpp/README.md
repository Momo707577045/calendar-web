# 任务栏时钟

这是一个简单的 Windows 任务栏时钟应用程序，可以在任务栏右侧显示当前时间和日期。

## 功能
- 当前环境为 windows 环境
- 悬浮 icon 时显示日期和时间
- 时间显示在第一行，日期显示在第二行
- 时间每秒自动更新
- 左键点击打开网页：https://blog.luckly-mjw.cn/tool-show/calendar-web/index.html
- 右键点击显示菜单，可以设置开机自启动或退出程序
- 支持开机自启动功能
- 使用项目中的 icon.ico 作为 exe 的图标

## 使用方法

1. 下载 `TaskbarClock.exe` 文件
2. 双击运行即可，时钟将显示在任务栏右侧
3. 左键点击时钟区域可以打开指定网页
4. 右键点击时钟区域可以显示菜单，可以设置开机自启动或退出程序

## 编译说明

### 方法一：使用提供的构建脚本（推荐）

1. 确保已安装 Visual Studio（2019 或 2022）和 CMake
2. 双击运行 `build.bat` 脚本
3. 编译完成后，将在项目根目录生成 `TaskbarClock.exe` 文件

### 方法二：使用 Visual Studio 和 CMake 手动编译

1. 安装 Visual Studio（任何版本都可以，如 Visual Studio 2019 或 2022）
2. 安装 CMake
3. 打开命令提示符或 PowerShell，导航到项目目录
4. 执行以下命令：
   ```
   mkdir build
   cd build
   cmake .. -A Win32
   cmake --build . --config Release
   ```
5. 编译完成后，可执行文件将位于 `build\bin\Release\TaskbarClock.exe`

## 技术说明

- 使用 Windows API 实现任务栏时钟功能
- 通过系统托盘图标实现时钟功能
- 使用注册表实现开机自启动功能
- 静态链接所有依赖，生成的可执行文件可以独立运行
- 不需要安装任何依赖，可以直接运行

## 注意事项

- 该程序仅适用于 Windows 操作系统
- 程序使用静态链接，生成的可执行文件尽可能小
- 如果遇到问题，可以尝试以管理员身份运行程序 