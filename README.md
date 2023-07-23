# CORDLE 自述文档

2023 年夏计算机程序设计基础（20740102）大作业

> Author: [杨哲涵](https://git.tsinghua.edu.cn/yangzheh22)

## 编译

`emcc -O3 -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['cwrap']" --shell-file html_template/shell_minimal.html api.cpp`

## 运行

`sudo python3 -m http.server 80`
