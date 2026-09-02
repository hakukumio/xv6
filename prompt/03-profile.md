# 03 — 学生档案

> 迁移自 `note/学习档案.md`（已废弃，本文件为权威）。
> 本文件是「静态档案」；进度类信息在 `04-progress.md`。

## 背景（固定，不常变）

- 会 C；自学过 CSAPP 前十章；没系统学过 OS。
- 没写过完整项目，工程经验少。
- 风格偏好：中文回答 + 英文术语；先结论后细节；列表/表格/代码块。

## 目标

- 在简明基础上扎实掌握 OS 原理，基本读懂 xv6 项目，同时补工程经验。
- 希望像 MIT 学生一样高效利用课程资源（reading/video/notes/boards/homework/lab 的完整闭环）。

## 已掌握（TA 据此调整讲解深度）

### OS 概念
- fork/pipe/exec/wait 系统调用链：`usys.S → syscall.c → sysproc.c → proc.c`（exec 在 sysfile.c）
- `read` 返回 0（EOF）的唯一条件 = 指向管道写端的所有 fd 都 close
- `filedup` = 引用计数，解决共享对象生命周期（不是「独享」）
- 目录 = 由 `struct dirent` 组成的文件（kernel/fs.h）；inode 的 `type` 区分文件/目录
- exec 的 `argv[0]` 必须 = 程序名（kernel/exec.c:79）
- 递归遍历文件的 fd 生命周期约束（NOFILE=16，kernel/param.h:3）
- 管道「字节流」vs 命令行「argv」的区别 → xargs 是适配器

### C 与工具
- 指针算术按类型缩放、数组名衰减、`3[a]==a[3]`、错位写内存的危害
- 读内核源码 5 步法 + grep 导航
- 条件编译 debug（`#ifdef DEBUG` + DPRINTF 宏）
- 增量测试法（把程序切成可独立验证的阶段）
- gdb 基础命令（断点/单步/x 内存/bt/watch/frame）——qemu-gdb 对接练习中
- git 日常（add/commit/branch/checkout/remote/show/stash）

## 薄弱点 / 反复错误（TA 要重点盯）

- ⚠️⚠️ **错误信息三要素**：`fprintf(2, "程序名: ...\n")`——已复发 **6 次**，最顽固
- fd 关闭规则：已显著改善，仍需持续检查（fork 后各端归属）
- 并发模型（sleep/wakeup、生产者-消费者闭环）尚未吃透，需后续加练
- 代码风格（缩进/空格/注释准确性）仍需对照 xv6 原生持续纠偏
- 习惯：跳过阅读直接做题 → 卡壳回头补（要用「阅读先行」纪律纠正）
