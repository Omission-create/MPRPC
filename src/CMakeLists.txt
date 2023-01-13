#将当前目录所有源文件放到源文件列表
aux_source_directory(./ SRC_LIST)
#muduo库编译成了静态库
# add_library(mprpc SHARED ${SRC_LIST})
add_library(mprpc SHARED ${SRC_LIST})
#muduo_net要在muduo_base之前
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)