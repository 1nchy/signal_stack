# Signal Stack

## 简介

传统的 C++ 信号操作较繁琐，涉及较多语句及变量，容易产生混淆乃至错误。我们提出的 `signal_stack` 在保留原有信号操作功能的基础上，大幅简化了信号操作所需要的步骤，并提供了基于栈的信号处理管理方式，适用于大多数信号处理的场景，能让开发者专注于主要功能的实现。

## 概述

`signal_stack` 是封装了信号处理数据的容器，它提供了一系列的方法，对特定信号（的处理函数）进行操作。`signal_stack` [接口](./include/signal_stack.hpp)。

`signal_stack` 仅使用默认构造函数。注意到，它的拷贝构造函数与赋值构造函数都被定义为删除，其背后的原因是显而易见的。一般来说，倘若对同一个进程，存在两个或多个不同的 `signal_stack` 对其信号处理进行管理，很容易引发错误，特别是当两个或多个 `signal_stack` 被**交替使用**时。

因此，我们建议通过**全局变量**的形式来使用该容器。如果我们不得不使用多个 `signal_stack` 管理信号时，请一定按栈的顺序来创建、销毁与操作。

### 使用

基于 `CMake` 的三方库使用，详情见 [1nchy/project_template](https://github.com/1nchy/project_template)。

### 示例

`system` 系统调用会产生一个 `SIGCHLD` 信号，在某些场景中是需要忽略的。详情见 [系统调用](./example/system)。

### 设计

详情见 [设计文档](./doc/design.md)。

## TODO
