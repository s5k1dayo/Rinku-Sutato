# Rinku Sutato — 高帧率低延迟手机投屏工具

基于 [scrcpy](https://scrcpyapp.org/) 和 [sndcpy](https://github.com/rom1v/sndcpy) 的 Windows 投屏前端，提供图形化按钮一键启动投屏与音频转发。

---

## 功能特性

- **高帧率低延迟** — 60 Hz 刷新率，约 30 ms 延迟
- **一键启动** — 图形界面，点击「启动!」即可投屏
- **音频同步** — 支持通过 sndcpy 转发手机音频到电脑
- **运行状态检测** — 实时显示 scrcpy / sndcpy 进程状态
- **一键关闭** — 关闭音频或按 `ESC` 退出程序

---

## 环境要求

| 项目 | 要求 |
|------|------|
| 操作系统 | Windows 10 / 11（**路径不含中文**） |
| 手机系统 | Android 5.0+，支持 USB 调试 |
| 连接方式 | USB 数据线（推荐）或 ADB 无线调试 |
| 前置依赖 | 已开启 [USB 调试](https://developer.android.com/studio/debug/dev-options) |

> 路径不能包含中文字符，否则 scrcpy 可能无法正常工作。

---

## 快速开始

### 首次使用

1. 手机开启 **USB 调试**，用数据线连接电脑
2. 在命令行执行以下命令，确认设备已连接：
   ```
   adb devices
   ```
   若列表中出现设备则表示连接成功
3. 双击 `scrcpy.exe`，确认投屏画面正常显示
4. 一切正常后，后续可直接使用 Rinku Sutato

### 日常使用

1. 手机连接电脑（USB 或 ADB 无线）
2. 双击 `Rinku Sutato.exe`
3. 点击 **「启动!」** 开始投屏
4. （可选）若无声音，点击 **「启动声音」** 转发手机音频
5. 按 **`ESC`** 退出程序

### 按钮说明

| 按钮 | 功能 | 对应脚本 |
|------|------|----------|
| 启动! | 启动 scrcpy 投屏 | `start.bat` |
| 启动声音 | 启动 sndcpy 音频转发 | `sndcpy.bat` |
| 关闭声音 | 关闭音频转发 | `exit.bat` |

---

## 启动参数

当前 `scrcpy` 默认启动参数如下：

```
scrcpy --max-fps=60 --video-bit-rate=8M --video-buffer=0 --video-codec=h264
```

如需自定义（如降低码率、限制分辨率），可编辑 `启动参数.txt` 修改。

### 常用参数参考

| 参数 | 作用 | 示例 |
|------|------|------|
| `--max-fps` | 限制帧率 | `--max-fps=30` |
| `--video-bit-rate` | 视频码率 | `--video-bit-rate=4M` |
| `--video-buffer` | 视频缓冲 (ms) | `--video-buffer=0` |
| `--video-codec` | 视频编码器 | `--video-codec=h265` |
| `-m` / `--max-size` | 限制画面分辨率 | `-m 1024` |

---

## 项目结构

```
Rinku Sutato sourse/
├── Rinku Sutato.cpp     # 主程序源码（EGE 图形界面）
├── scrcpy.exe           # scrcpy 投屏核心
├── sndcpy.bat           # 音频转发启动脚本
├── start.bat            # 投屏启动脚本
├── exit.bat             # 关闭音频脚本
├── scrcpy-server        # scrcpy 服务端 APK
├── sndcpy.apk           # sndcpy APK
├── adb.exe              # Android Debug Bridge
├── 启动参数.txt         # scrcpy 自定义参数
├── icon.png             # 程序图标
└── README.md
```

---

## 常见问题

**Q: 点击「启动!」后没有画面？**

确保手机已开启 USB 调试，且 `adb devices` 能识别到设备。首次使用请先用 `scrcpy.exe` 单独测试。

**Q: 没有声音？**

点击「启动声音」按钮。sndcpy 会向手机安装 APK，部分手机会提示不安全，选择「继续安装」即可。

**Q: 投屏卡顿/延迟高？**

可尝试降低码率（如 `--video-bit-rate=4M`）或限制帧率（`--max-fps=30`）。无线投屏延迟通常高于有线。

**Q: 提示找不到 ADB 设备？**

- 检查数据线是否支持数据传输（非纯充电线）
- 尝试更换 USB 接口
- 重启 ADB 服务：`adb kill-server && adb start-server`

---

## 致谢

- [scrcpy](https://scrcpyapp.org/) — 开源投屏核心
- [sndcpy](https://github.com/rom1v/sndcpy) — 音频转发工具

---

> Created by [s5k1dayo](https://space.bilibili.com/476112404) © 2026 · v1.0
>
> *为什么叫 Rinku Sutato？因为真的很想玩原神。*
