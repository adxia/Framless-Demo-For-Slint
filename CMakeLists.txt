cmake_minimum_required(VERSION 3.10)
project(SlintApp LANGUAGES CXX)

#设置 C++ 标准（Slint 需要 C++20 以获得最佳体验）
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Slint_DIR "E:/Slint/X86/Slint-X86/lib/cmake/Slint")
add_compile_options(/utf-8)
#查找 Slint 库
find_package(Slint REQUIRED)

# 如果用户没有指定构建类型（比如没写 -DCMAKE_BUILD_TYPE）
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # 设置可选范围，方便在 GUI 中选择
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_executable(SlintApp
src/main.cpp
)
target_include_directories(SlintApp PRIVATE src src/utils)
#编译 Slint 文件
slint_target_sources(SlintApp ui/main.slint)

# 链接 Slint 及其必要的库
target_link_libraries(SlintApp PRIVATE Slint::Slint)

if(WIN32)
    target_sources(SlintApp PRIVATE
        src/utils/EnableWindowShadow.cpp
        src/utils/EnableWindowShadow.h
    )
    #链接dwmapi
    target_link_libraries(SlintApp PRIVATE dwmapi user32)
    #复制dll
    add_custom_command(TARGET SlintApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "E:/Slint/X86/Slint-X86/lib/slint_cpp.dll"
        $<TARGET_FILE_DIR:SlintApp>
    )
    #隐藏控制台
    set_target_properties(SlintApp PROPERTIES WIN32_EXECUTABLE TRUE)
    set_target_properties(SlintApp PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()