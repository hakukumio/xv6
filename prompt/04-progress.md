# 04 — 进度与下一步

> **版本：2026-09-02**（每次会话结束更新此日期）
> TA 注意：本文件是**状态权威**，`01-handoff.md` 的快照可能滞后，以本文件为准。

## 进度总表

| Lab | 题目 | 状态 | 备注 |
|---|---|---|---|
| Lab util | sleep | ✅ 完成 | |
| Lab util | pingpong | ✅ 完成 | 曾因「冒号后缺空格」评分 0 分，已修；fd 复审通过 |
| Lab util | primes | ✅ 完成 | 6 个风格点已修 |
| Lab util | find | ✅ 完成 | off-by-one 长度检查已修 |
| Lab util | xargs | ✅ 完成 | `argv[0]=程序名` bug 已修 |
| Lab util | 封板 | ⏳ 待确认 | time.txt 已建，待全量 `make grade` 确认 100/100 |
| Lab syscall | trace / sysinfo | ⏳ 未启动 | 需先切 `syscall` 分支 |

## 课程位置

- 进度 ≈ 课表第 2 周末（sep 15/16 节点）：Lab util 完成、Lab syscall 该开工。
- LEC 1、LEC 2（C 部分完成，gdb 动手练习已布置未交报告）已覆盖。
- 下一步 = LEC 3（OS organization and system calls）预习 + Lab syscall。

## 下一步（按序勾选）

- [ ] ① Gitee push 修复（隐藏邮箱 hook：网页公开邮箱或改 commit email）后推送 util 分支
- [ ] ② 全量 `make grade` 确认 100/100
- [ ] ③ 补交 LEC2 gdb 动手观察报告（`x/20bx` 改前/改后快照 + `watch a[1]` 触发次数）
- [ ] ④ `git checkout syscall` + 确认 `user/trace.c`、`user/sysinfotest.c`、`conf/lab.mk`
- [ ] ⑤ LEC3 预习：book 第 2 章 + `kernel/proc.h`、`defs.h`、`entry.S`、`main.c`、`user/initcode.S`、`user/init.c`，skim `proc.c`/`exec.c`
- [ ] ⑥ 读 Lab syscall 官方 spec：https://pdos.csail.mit.edu/6.828/2021/labs/syscall.html
- [ ] ⑦ 写 trace
- [ ] ⑧ 写 sysinfo
- [ ] ⑨ 预习 LEC4（page tables），后续切 `pgtbl` 分支

## 最近会话记录

- 2026-09-02：Lab util 全量评分 99/100（差 time.txt）→ 修复 pingpong 空格 bug → 建立 Gitee 私有备份（遇「隐藏邮箱」hook 拦截）→ LEC 2 C 部分完成、gdb 动手课布置 → 建立本 prompt/ 交接包。
