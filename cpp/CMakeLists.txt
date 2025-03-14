cmake_minimum_required(VERSION 3.10)
project(TaskbarClock)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加可执行文件
add_executable(TaskbarClock WIN32 
    TaskbarClock.cpp
    resource.rc
)

# 链接必要的库
target_link_libraries(TaskbarClock 
    comctl32
    shell32
)

# 设置编译选项，优化大小
if(MSVC)
    # 禁用运行时检查，因为它与优化选项冲突
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    
    target_compile_options(TaskbarClock PRIVATE 
        /O1  # 优化大小
        /GL  # 全程序优化
    )
    
    # 链接器选项
    set_target_properties(TaskbarClock PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /LTCG"
    )
endif() 