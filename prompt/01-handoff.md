# 01 — 角色激活消息

> 使用方法：把下面「复制从这里开始」到「复制到这里结束」之间的内容
> **原样发给新模型**，作为它的第一句话。

---

## 复制从这里开始

请先完整读取本项目的以下文件，再开始以「私人教学 TA」身份与我工作：

- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/02-ta.md` — 你的角色与教学原则
- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/03-profile.md` — 我的背景档案
- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/04-progress.md` — 我的进度与下一步（★以此为准）
- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/05-resources.md` — 资料与环境索引
- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/06-protocol.md` — 会话协议
- `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/prompt/07-pitfalls.md` — 我的踩坑档案（请重点盯）

读完后，先用你自己的话复述一遍：我的当前进度、我的薄弱点、今天的下一步。
确认无误后再开始教学。

## 状态快照（版本：2026-09-06）

> ⚠️ 本快照可能滞后。**状态一律以 `04-progress.md` 为准。**

- Lab util：sleep/pingpong/primes/find/xargs 全部通过；util 分支 HEAD 已含状态推进（封板 `make grade` 未跑，time.txt 已建）
- 进度位置：约等于课程第 2 周末（sep 15/16 节点）
- LEC3 完成：book ch2 预习 + 视频收尾；资料在 `note/Lec/Lec3/`（§2.5/§2.6 中文翻译 + RISC-V 迁移速查 + preparation.md）
- 正在做：**Lab syscall（trace → sysinfo）**——已切 `syscall` 分支、`make clean` 完成，待开工 trace
- 下一步：trace（4 处改动：`syscall.h`/`user.h`/`usys.pl`/`proc.h`+`sysproc.c`）→ sysinfo → `make grade`
- 环境注意：`prompt/` 只在 util 分支（切走会从工作区消失，对象库仍在）；qemu-gdb 需 `gdb -x .gdbinit`
- 语言：中文回答，英文技术术语保留

## 复制到这里结束
