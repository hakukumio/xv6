# 05 — 资料与环境索引

> 所有相对路径基于 `/home/uuzfumo/MIT6S081/lab/xv6-labs-2020/`

## 教材与课程

| 资料 | 路径/链接 | 备注 |
|---|---|---|
| xv6 book | `note/book-riscv-rev2.pdf` | ★**只有 PDF！** 旧档案写 `.txt` 是错的 |
| 课程表 | `note/course.txt` | 完整周历（LEC/Homework/Assignment/DUE） |
| 官方 Schedule | https://pdos.csail.mit.edu/6.828/2021/schedule.html | |
| Lab specs | `tasks/lab0.html`（util，本地副本）；syscall 等需从官网抓 | |

## 讲义与笔记（note/）

- `note/Lab0.md` — Lab util 各题笔记
- `note/学习日志-2026-08-31-pipe与fork.md` — pipe/fork 总结（含错误修正表）
- `note/scratch0.md`、`note/xv6-makefile导读.md`
- `note/Lec/lec0/Lec0.md`、`Lec0-AI.md` — LEC0
- `note/Lec/Lec2/6S081-Intro-to-C-Fa21.pdf`、`gdb_slides.pdf`、`C&GDB.md` — LEC2
- （历史存档：`note/TA-prompt.md`、`note/学习档案.md` — 已迁移至 `prompt/`）

## 评分命令

- 单题：`make GRADEFLAGS=<sleep|pingpong|primes|find|xargs> grade`
- 全量：`make grade`（脚本 `grade-lab-util`）
- `time.txt`：单个整数（lab 耗时小时数），评分脚本要读它（1 分）

## 源码导览（kernel/）

| 主题 | 文件 |
|---|---|
| 系统调用链 | `usys.S → syscall.c → sysproc.c → proc.c`；exec 在 `sysfile.c:416` |
| 目录/inode | `fs.h`、`fs.c`（dirlookup:527、readi:456）、`stat.h`、`file.h` |
| 进程 | `proc.h`/`proc.c` |
| 常量 | `param.h`（NOFILE=16、MAXARG=32） |
| 管道 | `pipe.c` |
| Lab syscall 预习 | `kernel/proc.h, defs.h, entry.S, main.c, user/initcode.S, user/init.c` |
| Lab syscall 要改 | `kernel/syscall.c, syscall.h, sysproc.c, proc.c, Makefile, user/user.h, user/usys.pl` |

## 环境事实

| 项目 | 事实 |
|---|---|
| git 分支 | `util` @ 54ccaf8；本地另有 `syscall`(8813049)、`master`；远端有全部 lab 分支 |
| git 远程 | `origin` = MIT（只读）；`myrepo` = gitee.com/hakukumio/xv6.git（备份） |
| git 身份 | 曾用两个（cirnocube/zhongd430@gmail.com；hakukumio/18470434486@163.com）——建议统一 |
| 工具链 | `riscv64-linux-gnu-gcc`；`/usr/bin/gdb` 支持 riscv:rv64；`riscv64-linux-gnu-gdb` **未安装** |
| qemu-gdb | `make qemu-gdb` + `gdb -x .gdbinit`（Ubuntu 拒绝 auto-load，需显式 `-x` 或加 safe-path） |
| gdb 用户程序 | `file user/_xxx` → `b ...` → `c`，在 qemu 窗口触发 |

## xv6 printf 限制（写用户程序必知）

只支持 `%d %l %x %p %s %c %%`——**没有** `%.*s`、`%u`、宽度/精度（`user/printf.c:50`）。
