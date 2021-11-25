# webrtc目录介绍
1. api，api是整个webrtc的接口目录，提供对方访问的接口
2. call，管理层，通过不层可以对数据流进行管理
3. audio，存放音频网络逻辑层的代码，音频数据逻辑上的发送，接收等代码
4. video，和audio目录是一样，存放的是视频网络逻辑层的代码，包括视频数据逻辑层上的发送，接收
5. sdk，移动端Android/Ios的业务逻辑代码存放在这里，包括音视频采集和渲染等
6. pc，pc是peerconnection的简称，存放的是业务逻辑层的代码，比如track,source ,sink的关系
7. common_audio，存放一些音频的基本算法，包括环形队列，傅里叶变换，滤波器等等
8. common_video，存放常用的视频算法工具，如libyuv,sps/pps分析器,I420缓冲器
9. logging，日志目录
10. media，多媒体相关的逻辑，包括编码、解码的逻辑处理
11. modules，modules目录是webrtc工程里最重要的一个目录,因为包含众多模块
    - audio_coding 音频编解码器
    - audio_device 音频采集、播放模块
    - audio_mixer 混音模块，如多人互动音频传输
    - audio_processing 音频前处理和后处理模块，如回音消除、降噪、增益等
    - bitrate_controller 码率控制模块
    - congestion_controller 流控制模块，如在网络流量高时的具体处理
    - desktop_capture 桌面采集模块
    - pacing 码率探测器及平滑处理模块
    - remote_bitrate_estimator 远端码率探测器
    -  rtp_rtcp rtp/rtcp协议相关模块
    - third_party
    - utility
    - video_capture 视频采集模块
    - video_coding 视频编解码模块，如h264等
    - video_processing 视频前处理和后处理模块
12.  rtc_base，类似于基础操作类库，封装了很多基本代码，比如线程，event,socket等相关代码，一般刚开始建议花点时间琢磨下这部分的代码，后面对于代码的深入理解有帮助
13.  rtc_tool，工具目录，包含音频、视频等分析工具
14.  tool_webrtc，WebRTC相关测试工具目录，如网络测试、音视频测试等
15.  system_wrapper，与操作系统相关的代码，如CPU特性，原子操作，读写锁，时钟等
16.  p2p，P2P穿透相关，turn/stun等，服务器和客户端
17.  stats，统计数据目录，包含丢包率、网络抖动等统计数据

# 编译
```
gn gen out/Default --args='target_os="linux" target_cpu="x64"'
ninja -C out/Default
-C选项告诉ninja，进入out/Default目录来编译。所以，它等同于：
cd out/Release;ninja
```

# 使用历史版本
```
git reset --hard xxxxxID
gclient config https://webrtc.googlesource.com/src.git
gclient sync
```
- webrtc项目是用gclient来管理源码的checkout, update等。
- gclient是google专门为这种多源项目编写的脚本，它可以将多个源码管理系统中的代码放在一起管理。甚至包括将git和svn代码放在一起。
- 和gclient密切相关的两类文件.gclient和DEPS:
   - .gclient文件是gclient的控制文件，该文件放在工作目录的最上层。".gclient"文件是一个Python的脚本
   ```
   solutions = [ 
   { 
      "name"        : "trunk",
      "url"         : "http://webrtc.googlecode.com/svn/trunk",
      "deps_file"   : "DEPS",
      "managed"     : True,
      "custom_deps" : {},
      "safesync_url": "",
   },
   ]
   cache_dir = None
   ```
   - DEPS也是一个python脚本，最简单的，如下：
   ```
   deps = {  
   "src/outside" : "http://outside-server/trunk@1234",  
   "src/component" : "svn://svnserver/component/trunk/src@77829",  
   "src/relative" : "/trunk/src@77829",  
   }  
   ```

# 遇到的问题
## 两个peerconnection_client互联，被连接的client崩溃
```
现象：
# Fatal error in: ../../rtc_base/physical_socket_server.cc, line 1171
# last system error: 11
# Check failed: !waiting_
# Aborted (core dumped)

解决办法1：
不使用debug版本
is_debug = false

解决办法2：
回退到81bbd7199a2e97680a4488c2da4f8248137e12e0版本（别忘记gclient sync）
```
