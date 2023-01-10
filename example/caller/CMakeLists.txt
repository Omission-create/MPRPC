# aux_source_directory(./ SRC_LIST)
# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)
#从指定源文件列表构建出可执行目标文件
add_executable(consumer ${SRC_LIST})
#添加可执行目标文件的动态连接库
target_link_libraries(consumer mprpc protobuf pthread)