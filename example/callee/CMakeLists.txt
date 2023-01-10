# aux_source_directory(./ SRC_LIST)
# set(SRC_LIST userservice.cc ../user.pb.cc)
set(SRC_LIST friendservice.cc ../friend.pb.cc)
#从指定源文件列表构建出可执行目标文件
add_executable(provider ${SRC_LIST})
#添加可执行目标文件的动态连接库
target_link_libraries(provider mprpc protobuf pthread)