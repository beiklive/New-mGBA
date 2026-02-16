
message(STATUS "this is New_mGBA CMakeLists.txt")


list(APPEND APP_PLATFORM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE NEWMGBA_SRC src/*.cpp)







# 添加到主程序源文件列表
list(APPEND MAIN_SRC ${NEWMGBA_SRC})
