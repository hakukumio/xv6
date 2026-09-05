# 04 — 进度与下一步

> **版本：2026-09-06**（每次会话结束更新此日期）
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

- 进度 ≈ 课表第 2 周末（sep 15/16 节点）：Lab util 完成、Lab syscall 开工中。
- LEC 1–3 已覆盖（LEC2 的 gdb 动手报告未交；LEC3 预习 + 视频完成）。
- 当前 = Lab syscall（trace → sysinfo）。

## 下一步（按序勾选）

- [x] ① Gitee push：2026-09-03 核对 `myrepo/util` = HEAD `71552ac`，util 分支已同步（未来 push 若再遇隐藏邮箱 hook：网页公开邮箱或改 commit email）
- [ ] ② 全量 `make grade` 确认 100/100
- [ ] ③ 补交 LEC2 gdb 动手观察报告（`x/20bx` 改前/改后快照 + `watch a[1]` 触发次数）
- [ ] ④ `git checkout syscall` + 确认 `user/trace.c`、`user/sysinfotest.c`、`conf/lab.mk`
- [x] ⑤ LEC3 预习：book 第 2 章（§2.1–2.6 已全通读）+ `proc.h`/`defs.h`/`entry.S`/`main.c`/`initcode.S`/`init.c`；§2.6 复述**验收通过**（特权级时间线 M→S→U→S→U 已补）
  - 配套资料：`note/Lec/Lec3/book-ch2.5-中文翻译.md`、`book-ch2.6-中文翻译.md`、`riscv-迁移速查-从x86CSAPP.md`
- [x] ⑤' LEC3 视频收尾：已观看（中文版 01:20:52），视频后「用户态/内核态」5 问自答写入 `note/Lec/Lec3/preparation.md` §1，TA 批改**验收通过**（笔记未单独建 Lec3.md，并入 preparation.md）
- [ ] ⑥ 读 Lab syscall 官方 spec：https://pdos.csail.mit.edu/6.828/2021/labs/syscall.html
- [ ] ⑦ 写 trace
- [ ] ⑧ 写 sysinfo
- [ ] ⑨ 预习 LEC4（page tables），后续切 `pgtbl` 分支

## 最近会话记录

- 2026-09-02：Lab util 全量评分 99/100（差 time.txt）→ 修复 pingpong 空格 bug → 建立 Gitee 私有备份（遇「隐藏邮箱」hook 拦截）→ LEC 2 C 部分完成、gdb 动手课布置 → 建立本 prompt/ 交接包。
- 2026-09-03（LEC3 预习会话）：§2.1–2.5 通读答疑；产出 `note/Lec/Lec3/book-ch2.5-中文翻译.md`（逐段翻译+仓库行号锚点）与 `note/Lec/Lec3/riscv-迁移速查-从x86CSAPP.md`；核对确认 `myrepo/util` 已同步（①勾掉）。**遗留：§2.6 未读、syscall 分支未切、Lab util 封板 make grade 未跑、LEC2 gdb 报告未交。**
- 2026-09-04：§2.6 中文翻译存档 + 陪读走查；§2.6 复述逐句批改**验收通过**（补 userinit「造车」vs scheduler「点火」、mret=mepc+MPP 两处措辞；补特权级时间线 M→S→U→S→U）；产出 `note/Lec/Lec3/book-ch2.6-中文翻译.md`。决定下一步：看 LEC3 视频收尾。遗留：视频未看、syscall 分支未切、make grade 封板未跑、gdb 报告未交。
- 2026-09-05/09-06：看完 LEC3 视频（01:20:52）；`preparation.md` §1 写「用户态/内核态」5 问自答 → TA 批改通过（补 RISC-V 词汇：satp/sfence.vma/ecall vs x86 的 CR3/syscall）；抓取 Lab syscall spec；核验 syscall 分支（trace.c/sysinfotest.c 齐备、SYS_* 只到 21、user.h/usys.pl 无入口）→ **切 syscall 分支开工**。
