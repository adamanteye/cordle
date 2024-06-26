# CORDLE 自述文档

2023 年夏计算机程序设计基础（20740102）大作业

> Author: [杨哲涵](https://git.tsinghua.edu.cn/yangzheh22)

## 编译

`emcc -O3 -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['cwrap']" api.cpp`

## 运行

`sudo python3 -m http.server 80` 或[在线演示](https://game.thudep22.cn/cordle/)

## 程序结构和说明

- `wordle.hpp` Wordle C++ 库
  - `builtin_words.hpp` 存储候选词与答案词
  - `Round` 实现单局游戏逻辑，即每次猜测后输入与键盘的状态
    - `WordSet` 读取并构建词库，同时判断输入是否合法
    - `Keyboard` 实现键盘状态的维护
- `api.cpp` 通过 C++ WebAssembly 将 api 函数暴露出来以供 Javascript 调用
- `index.html` 处理用户输入输出，是 GUI 界面的实现，通过回调函数实时渲染游戏界面

## 完成作业的感想

1. 不要过早优化：在写可通过 oj 的程序时，并没有把项目如何组织想得非常明白，但还是按照自己的预判写了一堆代码。事后看来，这些代码虽然可以重用，但实际上 `wordle.hpp` 中的实例层级可以组织得更清晰一点。导致在完成提高部分的时候仍然需要反复递归查看定义，实现等等，并没有达到抽象封装的目的，反而增加了很多思维负担。
2. 量力而行：在确定要实现一个 Web 界面的 GUI 后，我开始寻找前端框架。在浪费了一个上午后意识到 Vue 的语法和我目前接触到的编程语言都不太相同，并且这并不是一个很大的项目，简单的 html css js 就应该够用了。于是最后花了一天多的时间从菜鸟教程，各种 csdn 例子中学习 js，最后完成了这份 `index.html`。
3. 选择合适的框架：由于我选择了 WebAssembly，因此不必学习其他 GUI 实现方法中如何绘制界面，仅需要写好网页即可，这省去了不少时间，并且有不少现成的文档和例子可供参考。此外，这样的实现方式对于自动化构建发布也很方便。
