set(eapp_bin mobilenet-split-three)
set(eapp_src eapp/split1.cpp eapp/Network1.cpp eapp/reluLayer.cpp eapp/batchnormalLayer.cpp eapp/convLayer.cpp eapp/readdata1.cpp eapp/utils1.cpp eapp/layers_bn.cpp eapp/crypto.cpp eapp/aes.c)
set(host_bin mobilenet-split-three-runner)
set(host_src host/host.cpp host/aes.c)

set(a eapp/string.c)
set(c eapp/syscall.c)
set(d eapp/encret.s)

set(eapp_bin1 mobilenet-split-three-2)
set(eapp_src1 eapp/split2.cpp eapp/Network2.cpp eapp/fcLayer.cpp eapp/reluLayer.cpp eapp/layers_ds.cpp eapp/batchnormalLayer2.cpp eapp/sigmoidLayer.cpp eapp/convLayer2.cpp eapp/utils2.cpp eapp/layers_bn2.cpp eapp/globalpoolLayer.cpp eapp/crypto.cpp eapp/aes.c)

set(eapp_bin2 mobilenet-split-three-3)
set(eapp_src2 eapp/split3.cpp eapp/Network3.cpp eapp/fcLayer.cpp eapp/reluLayer.cpp eapp/layers_ds.cpp eapp/batchnormalLayer2.cpp eapp/sigmoidLayer.cpp eapp/convLayer2.cpp eapp/utils3.cpp eapp/layers_bn2.cpp eapp/globalpoolLayer.cpp eapp/crypto.cpp eapp/aes.c)

set(package_name "mobilenet-split-three.ke")
set(package_script "./mobilenet-split-three-runner mobilenet-split-three mobilenet-split-three-2 mobilenet-split-three-3 eyrie-rt --sm-bin fw_jump.bin")

if(RISCV32)
  set(eyrie_plugins "freemem rv32 io_syscall linux_syscall env_setup")
else()
  set(eyrie_plugins "freemem io_syscall linux_syscall env_setup")
endif()

# eapp

add_executable(${eapp_bin} ${eapp_src} ${a} ${c} ${d})
target_link_libraries(${eapp_bin} "-static")
target_include_directories(${eapp_bin}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/app
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge)
  
add_executable(${eapp_bin1} ${eapp_src1} ${a} ${c} ${d})
target_link_libraries(${eapp_bin1} "-static")
target_include_directories(${eapp_bin1}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/app
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge)
  
add_executable(${eapp_bin2} ${eapp_src2} ${a} ${c} ${d})
target_link_libraries(${eapp_bin2} "-static")
target_include_directories(${eapp_bin2}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/app
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge)

# host

add_executable(${host_bin} ${host_src})
target_link_libraries(${host_bin}
  ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_EDGE} ${KEYSTONE_LIB_VERIFIER} pthread)
set_target_properties(${host_bin}
  PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
  )
target_include_directories(${host_bin}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/common
  PUBLIC ${KEYSTONE_SDK_DIR}/include/host
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge
  PUBLIC ${KEYSTONE_SDK_DIR}/include/verifier)

# add target for Eyrie runtime (see keystone.cmake)

set(eyrie_files_to_copy .options_log eyrie-rt)
add_eyrie_runtime(${eapp_bin}-eyrie
  ${eyrie_plugins}
  ${eyrie_files_to_copy})

# add target for packaging (see keystone.cmake)

add_keystone_package(${eapp_bin}-package
  ${package_name}
  ${package_script}
  ${eyrie_files_to_copy} ${eapp_bin} ${eapp_bin1} ${eapp_bin2} ${host_bin} ${fw_bin} ${image})

add_dependencies(${eapp_bin}-package ${eapp_bin}-eyrie)

# add package to the top-level target
add_dependencies(examples ${eapp_bin}-package)
