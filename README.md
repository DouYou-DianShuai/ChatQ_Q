# ChatQ_Q
辞职转行选手，为写简历简单撸点小代码，初步找工作用，不然简历太空又没相关工作经验的话不太好整。

##### 简单说一下:

* 服务端：使用基于Tcp并发的编程方式，模仿QQ对一些基础功能 做了简单的实现，如：好友功能、聊天功能（好友聊天 或 群聊）、简单的文件传输功能等等......

* 客户端: 使用Qt的socket网络编程，事件，信号与槽，基本控件等Qt框架资源 + 简单的逻辑代码实现。

* 其他：交互使用json，自己做协议方案；数据持久化使用mysql数据库。

##### 总结下实现思路：
* 基于Tcp并发的实现方法很多，多进程，多线程，io多路复用等都可以。最后采用了epoll+多线程地模式。

 * 由于聊天功能实时性要求相对较低，而进程开销过大，用多线程的话达到一定数量时的总的上下文切换浪费时间会很大，并发能力一定程度上比不上io复用，故采用epoll的方式实现，但由于文件传输有可能影响正常聊天，故使用新线程新连接的方式独立出去传输，用到了之前写地线程池。
 * 最后由于对epoll的理解可能不太够地原因，总会产生一些小错误，一些细节很难覆盖的到。/*比如举个小例子，epoll采用边沿触发，然后使用非阻塞 io，这时调用 send 发送数据返回-1 时，就需要对全局变量 errno 的错误信 息进行处理，若是 EAGAIN 为缓冲区已满，这时就需要记录写入位置方便 epoll 对缓存区可写状态进行监测与通知后，高效稳定的 发送数据信息。*/
 * 因此服务框架就直接使用了libevent开源库封装好的epoll来快速编写，随后把精力更多的投入逻辑代码的实现。
 * 对于epoll地学习与练习，还有一些其他高性能优秀开源服务器框架源码的学习，即使工作了也不能懈怠，争取不断地提升自我。

##### 如果打算运行一下本项目

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
1、不难看出，在Linux下打开项目后，进入server/build目录，执行make即可，执行的是用cmake生成的Makefile文件。

       由于项目简单内容较少，直接自己 gcc/g++ 也无可厚非

2、mysql需准备，名为 userName   和   groupChatName  两个数据库

3、运行第一步make编译后生成的可执行程序    ./Q_Q-Server  

   到此，服务器正常启动运行。
****
Linux使用Qt打开我写的client工程源码，编译运行后，注册/登录不受影响即一切正常

或使用我提供的windows、安卓环境的可执行程序来安装（自己复制源码到对应平台编译也可，Qt是跨平台的）


##### 项目展示
* 服务器启动
<!--   
![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E6%9C%8D%E5%8A%A1%E5%99%A8%E5%90%AF%E5%8A%A8%E5%9B%BE.png) -->

![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E6%9C%8D%E5%8A%A1%E5%99%A8%E5%90%AF%E5%8A%A8%E5%9B%BE.png)
   
****
* 登陆窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E7%99%BB%E5%BD%95%E7%95%8C%E9%9D%A2.png) -->
   
![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E7%99%BB%E5%BD%95%E7%95%8C%E9%9D%A2.png)

`客户端启动失败的话，登录窗口是无法操作的，如下图:`

<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E5%A6%82%E6%9E%9C%E6%B2%A1%E8%83%BD%E6%88%90%E5%8A%9F%E8%BF%9E%E6%8E%A5%E6%9C%8D%E5%8A%A1%E5%99%A8%EF%BC%8C%E6%88%96%E6%9C%8D%E5%8A%A1%E5%99%A8%E4%B8%8D%E5%9C%A8%E7%BA%BF%EF%BC%8C%E5%88%99%E6%98%BE%E7%A4%BA%E4%BB%A5%E4%B8%8B%E4%B8%8D%E5%8F%AF%E6%93%8D%E4%BD%9C%E7%95%8C%E9%9D%A2%EF%BC%8C%E5%8F%AF%E4%BB%A5%E6%A3%80%E6%9F%A5%E4%B8%8Bip%E5%9C%B0%E5%9D%80.png) -->

![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E5%A6%82%E6%9E%9C%E6%B2%A1%E8%83%BD%E6%88%90%E5%8A%9F%E8%BF%9E%E6%8E%A5%E6%9C%8D%E5%8A%A1%E5%99%A8%EF%BC%8C%E6%88%96%E6%9C%8D%E5%8A%A1%E5%99%A8%E4%B8%8D%E5%9C%A8%E7%BA%BF%EF%BC%8C%E5%88%99%E6%98%BE%E7%A4%BA%E4%BB%A5%E4%B8%8B%E4%B8%8D%E5%8F%AF%E6%93%8D%E4%BD%9C%E7%95%8C%E9%9D%A2%EF%BC%8C%E5%8F%AF%E4%BB%A5%E6%A3%80%E6%9F%A5%E4%B8%8Bip%E5%9C%B0%E5%9D%80.png)

`是因为我在客户端作了判断，与服务器通信不上就使控件失能。`

****
* 注册窗口

<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E6%B3%A8%E5%86%8C%E7%95%8C%E9%9D%A2.png) -->

![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E6%B3%A8%E5%86%8C%E7%95%8C%E9%9D%A2.png)

****
* 聊天室主窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E8%81%8A%E5%A4%A9%E5%AE%A4%E7%95%8C%E9%9D%A2.png) -->


![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E8%81%8A%E5%A4%A9%E5%AE%A4%E7%95%8C%E9%9D%A2.png)
   
****
* 好友聊天窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E5%A5%BD%E5%8F%8B%E8%81%8A%E5%A4%A9%E7%95%8C%E9%9D%A2.png) -->
   
![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E5%A5%BD%E5%8F%8B%E8%81%8A%E5%A4%A9%E7%95%8C%E9%9D%A2.png)

****
* 群聊窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E7%BE%A4%E8%81%8A%E7%95%8C%E9%9D%A2.png) -->
   
![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E7%BE%A4%E8%81%8A%E7%95%8C%E9%9D%A2.png)

****
* 分别是添加好友 、创建群聊 、申请加群窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E5%88%86%E5%88%AB%E6%98%AF%E6%B7%BB%E5%8A%A0%E5%A5%BD%E5%8F%8B%E3%80%81%E5%88%9B%E5%BB%BA%E7%BE%A4%E8%81%8A%E3%80%81%E7%94%B3%E8%AF%B7%E5%8A%A0%E7%BE%A4%E7%9A%84%E7%AA%97%E5%8F%A3.png) -->
   
![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E5%88%86%E5%88%AB%E6%98%AF%E6%B7%BB%E5%8A%A0%E5%A5%BD%E5%8F%8B%E3%80%81%E5%88%9B%E5%BB%BA%E7%BE%A4%E8%81%8A%E3%80%81%E7%94%B3%E8%AF%B7%E5%8A%A0%E7%BE%A4%E7%9A%84%E7%AA%97%E5%8F%A3.png)

****
* 发送文件窗口
  
<!-- ![image](https://github.com/DouYou-DianShuai/ChatQ_Q/blob/main/picture/%E5%8F%91%E9%80%81%E6%96%87%E4%BB%B6.png) -->

![image](https://gitee.com/DouYou-DianShuai/ChatQ_Q/raw/main/picture/%E5%8F%91%E9%80%81%E6%96%87%E4%BB%B6.png)
   
****













