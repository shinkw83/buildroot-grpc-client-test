cmake_minimum_required(VERSION 3.5)

project(grpc-cli C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
  src/main.cpp 
  src/device_control.grpc.pb.cc
  src/device_control.pb.cc
)

target_link_libraries(${PROJECT_NAME} 
  protobuf::libprotobuf
  gRPC::grpc++_reflection
  gRPC::grpc++
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

