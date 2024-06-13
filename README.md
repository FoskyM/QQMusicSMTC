# 弃坑说明
2024/06/13  QQ 音乐在最新的 20.22.2692.0613 版本中增加了 SMTC 的支持。

# QQMusicSMTC
让你的 QQ 音乐支持 SMTC 协议。\
![image](https://github.com/FoskyM/QQMusicSMTC/assets/39661663/6323452a-f7bc-4105-9040-33b575893682)


## 章节
- [写在前面](#写在前面)
- [支持版本](#支持版本)
- [使用方法](#使用方法)
- [存在的问题](#存在的问题)
- [要做的](#要做的)
- [感谢项目](#感谢项目)
- [应用场景](#应用场景)

## 写在前面
- 什么是 SMTC ？\
  SMTC（SystemMediaTransportControls）是在 Windows 8.1 中引入的 API 集。其效果如上图所示。
- 有什么用？\
  右下角这个小东西的状态可以被其它应用获取到，便有了可玩性。详情请看末尾应用场景。

## 支持版本
- QQMusic 2013.19.03.03 （本插件发布时该版本为最新版）

## 使用方法
- 下载 Release 中的 `QQMusicSMTC_DLL_With_EXE.dll` 与 `QQMusicSMTC.exe`，\
- 打开 QQ 音乐的目录，比如 `E:\Program Files\Tencent\QQMusic\QQMusic2013.19.03.03\`，\
- 把两个文件丢进去，并把 `QQMusicSMTC_DLL_With_EXE.dll` 重命名为 `msimg32.dll`，\
- 启动 QQ 音乐，查看右下角媒体播放器状态确认是否成功。

## 存在的问题
- 网络波动或不良状况导致的歌曲信息获取不到，导致空白
- 有时会卡在某首歌（有时间再修）

## 要做的
- [x] 支持 SMTC
- [ ] 根据客户端版本自动切换基址偏移
- [ ] 单文件 dll （不知道为什么单文件 dll 创建 MediaPlayer 对象时就没反应了，有空再研究）
- [ ] 补上缺失的应用信息（估计改成单文件 dll 的方式就可以了）
- [ ] 音乐信息从内存获取而非网络（由于 CE 捣鼓了很久，只找到 ID 和 时长 两个固定地址，所以另辟蹊径）
- [ ] 优化播放状态的获取（目前通过 AudioSession 获取，播放状态是不会有延时的，但暂停后要过好多秒，AudioSession 才会从播放状态变为暂停）
- [ ] 优化切歌的判断（目前通过判断内存中的音乐 ID 来判断切歌，需要反复扫描内存，加上延时又会导致切歌判断的不及时）
- [ ] 按钮事件的响应（Q音是 GDI 绘制的，常规 Win32 Api 调用按钮的方法应该是行不通；内存调用我也不会）

## 原理
反复从内存中读取当前音乐 ID，通过网页版 QQ 音乐获取音乐信息数据，推送到 SMTC。
### 2013.19.03.03
MusicSongId -> QQMusic.dll+B06778 \
MusicTotalTime -> QQMusic.dll+B0678C -> QQMusic.dll+B06778+14

## 感谢项目
- [BetterNCM/InfinityLink](https://github.com/BetterNCM/InfinityLink)
- [MicroCBer/BetterNCM](https://github.com/MicroCBer/BetterNCM)
- [strivexjun/AheadLib-x86-x64](https://github.com/strivexjun/AheadLib-x86-x64)
- [Kxnrl/NetEase-Cloud-Music-DiscordRPC](https://github.com/Kxnrl/NetEase-Cloud-Music-DiscordRPC)

## 应用场景
- **Wallpaper Engine：壁纸引擎** \
  音频响应壁纸可从 SMTC 协议中获取正在播放的音乐信息。
  ![image](https://github.com/FoskyM/QQMusicSMTC/assets/39661663/4778e63e-cd80-4b75-abfe-7d51fc3cac57)
- **Mix Space & Shiro & ProcessReporter** \
  [ProcessReporter](https://mx-space.js.org/themes/shiro/extra#%E6%88%91%E7%9A%84%E5%8A%A8%E6%80%81) 可以通过 SMTC 协议获取到你在听的音乐，上报到 [Mix Space](https://github.com/mx-space) 的接口，并在其 [Shiro](https://github.com/Innei/Shiro) 主题上显示。 \
  ![image](https://github.com/FoskyM/QQMusicSMTC/assets/39661663/be0772e4-51da-42a4-8b7f-a759288ec80f)

