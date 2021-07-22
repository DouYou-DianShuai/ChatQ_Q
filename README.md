# ChatQ_Q
辞职转行选手，为写简历简单撸点小代码，初步找工作用，不然简历太空又没相关工作经验的话不太好整。

`简单说一下`

* 服务端：使用基于Tcp并发的编程方式，模仿QQ对一些基础功能 做了简单的实现，如：好友功能、聊天功能（好友聊天 或 群聊）、简单的文件传输功能等等......

* 客户端: 使用Qt的socket网络编程，事件，信号与槽，基本控件等Qt框架资源 + 简单的逻辑代码实现。

* 其他：交互使用json，自己做协议方案；数据持久化使用mysql数据库。

`最后总结：`
* 基于Tcp并发的实现方法很多，多进程，多线程，io多路复用等都可以。最后采用了epoll+多线程地模式。

 * 由于聊天功能实时性要求相对较低，而进程开销过大，用多线程的话达到一定数量时的总的上下文切换浪费时间会很大，并发能力一定程度上比不上io复用，故采用epoll的方式实现，但由于文件传输有可能影响正常聊天，故使用新线程新连接的方式独立出去传输，用到了之前写地线程池。
 * 最后由于对epoll的理解可能不太够地原因，总会产生一些小错误，一些细节很难覆盖的到。/*比如举个小例子，epoll采用边沿触发，然后使用非阻塞 io，这时调用 send 发送数据返回-1 时，就需要对全局变量 errno 的错误信 息进行处理，若是 EAGAIN 为缓冲区已满，这时就需要记录写入位置方便 epoll 对缓存区可写状态进行监测与通知后，高效稳定的 发送数据信息。*/
 * 因此服务框架就直接使用了libevent开源库封装好的epoll来快速编写，随后把精力更多的投入逻辑代码的实现。
 * 对于epoll地学习与练习，还有一些其他高性能优秀开源服务器框架源码的学习，即使工作了也不能懈怠，争取不断地提升自我。

***如果打算运行一下本项目***

先来看一下server的目录结构
```
server/
├── build
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 3.16.3
│   │   ├── cmake.check_cache
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── CMakeOutput.log
│   │   ├── CMakeTmp
│   │   ├── Makefile2
│   │   ├── Makefile.cmake
│   │   ├── progress.marks
│   │   ├── Q_Q-Server.dir
│   │   └── TargetDirectories.txt
│   ├── cmake_install.cmake
│   ├── compile_commands.json
│   └── Makefile
├── CMakeLists.txt
├── databaseQ_Q.cpp
├── databaseQ_Q.h
├── listQ_Q.cpp
├── listQ_Q.h
├── main.cpp
├── serverQ_Q.cpp
├── serverQ_Q.h
├── threadpool.c
└── threadpool.h
```
1、不难看出，在Linux下打开项目后，进入server/build目录，执行make即可

       由于项目简单内容较少，直接自己 gcc/g++ 也无可厚非

2、mysql需准备名为 userName   和   groupChatName  的数据库

3、运行第一步编译后生成的  Q_Q-Server

   到此，服务器正常启动运行。
****
Linux使用Qt打开client工程，编译运行后即可通过服务器正常使用
或使用我提供的windows、安卓环境的可执行程序来安装（自己复制源码到对应平台编译也可，Qt是跨平台的）
