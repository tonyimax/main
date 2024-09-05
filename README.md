使用CLion编辑器或者CMake命令行编译<br>
配置与生成: cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug<br>
编译:      cmake --build build --config Debug<br>
生成main,server,client三个可执行文件在build文件夹中<br>
./build/server启动服务端<br>
./build/client启动客户端<br>
