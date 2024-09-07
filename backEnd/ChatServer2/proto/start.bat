@echo off
protoc --proto_path=. --cpp_out=. message.proto
protoc --proto_path=. --grpc_out=. --plugin=protoc-gen-grpc="D:\develop_tools\cppsoft\vcpkg\packages\grpc_x64-windows\tools\grpc\grpc_cpp_plugin.exe" message.proto