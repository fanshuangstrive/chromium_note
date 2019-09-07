# cronet.note

chrominum cronet

chrominum net

GQUIC



Chromium 源码检索： https://cs.chromium.org/chromium/src/

####  Chromium Cronet 

 开发文档 https://chromium.googlesource.com/chromium/src/+/refs/heads/master/components/cronet

- C[ronet android](https://chromium.googlesource.com/chromium/src/+/refs/heads/master/components/cronet/android)

编译 android 库(Ubuntu 18.04)：https://chromium.googlesource.com/chromium/src/+/refs/heads/master/docs/android_build_instructions.md

android 开发文档  https://developer.android.com/guide/topics/connectivity/cronet

android 示例代码 https://chromium.googlesource.com/chromium/src/+/refs/heads/master/components/cronet/android

[https://github.com/GoogleChromeLabs/cronet-sample/ ](https://github.com/GoogleChromeLabs/cronet-sample/)

- C[ronet ios](https://chromium.googlesource.com/chromium/src/+/refs/heads/master/components/cronet/ios/)

编译ios 库（Mac）：https://github.com/chromium/chromium/blob/master/docs/mac_build_instructions.md

ios 示例代码：https://chromium.googlesource.com/chromium/src/+/refs/heads/master/components/cronet/ios/

#### 

------

#### Chromium net 

[quic_client ](https://cs.chromium.org/chromium/src/net/tools/quic/)

[epoll_quic_client](https://cs.chromium.org/chromium/src/net/third_party/quiche/src/quic/tools/)

------

### Chrome浏览器启用quic

1.启用quic： chrome://flags/#enable-quic

2.检查quic：

chrome://net-internals/#quic 

安装 [http2-and-spdy-indicator](https://chrome.google.com/webstore/detail/http2-and-spdy-indicator/mpbpobfflnpcgagjijhmgnchggcjblin) 浏览器插件 

抓包
支持 QUIC 的网站： [QUIC Discovery](https://docs.google.com/document/d/1i4m7DbrWGgXafHxwl8SwIusY2ELUe8WX258xt2LFxPM/edit?pref=2&pli=1#heading=h.dk2fhev07ryt)  