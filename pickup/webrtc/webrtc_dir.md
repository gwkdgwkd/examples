### webrtc目录介绍

1. api目录
   api是整个webrtc的接口目录，提供对方访问的接口.

2. audio目录
   存放音频网络逻辑层的代码，音频数据逻辑上的发送，接收等代码.

3. video目录
   和audio目录是一样，存放的是视频网络逻辑层的代码，包括视频数据逻辑层上的发送，接收.

4. sdk目录
   移动端Android/Ios的业务逻辑代码存放在这里.

5. pc目录
   pc是peerconnection的简称，存放的是业务逻辑层的代码，比如track,source ,sink的关系.

6. common_audio目录
   存放一些音频的基本算法，包括环形队列，傅里叶变换，滤波器等等.

7. common_video目录
   存放常用的视频算法工具，如libyuv,sps/pps分析器,I420缓冲器.

8. modules目录
   modules目录是webrtc工程里最重要的一个目录,因为包含很多分之，所有具体的任务我们后面再详细介绍.

9. rtc_base目录
   类似于基础操作类库，封装了很多基本代码，比如线程，event,socket等相关代码，一般刚开始建议花点时间琢磨下这部分的代码，后面对于代码的深入理解有帮助.

10. system_wrapper目录
    与操作系统相关的代码,如CPU特性，原子操作，读写锁，时钟等.

12. p2p目录
    P2P穿透相关，turn/stun等，服务器和客户端.
