# DDTranscoding
## 介绍
这是一个可以实现在线转码的转格式的服务器，只需要通过HTTP服务把源文件和目标文件设定好就可以实现在线转码。目前主要是用于RTSP转RTMP的拉流转码的功能。
## 使用方法

* 新增转码：post json http://localhost:4253/add

```
{
	"src":	"rtsp://xxxxxx/xxx",
	"dst":	"rtmp://xxxx/test"
}
```

* 删除转码：post json http://localhost:4253/del

```
{
	"dst":	"rtmp://xxxx/test"
}
```

* 获取所有正在执行的任务：post json http://localhost:4253/list

```
{
}
```

## 编译方法
* 安装所有依赖的库：libffmpeg、libcjson、libssl、libcrypto、libevent
* 确保所有的库都能被pkg-config找到
* make
* DDTranscoding就是可执行文件