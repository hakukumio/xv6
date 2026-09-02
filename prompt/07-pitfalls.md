# 07 — 踩坑档案（TA 重点盯）

> **版本：2026-09-02**

## 反复错误（最高优先级）

| # | 错误类型 | 正确写法 | 复发次数 | 状态 |
|---|---|---|---|---|
| 1 | 错误信息三要素：写到 stdout / 漏 `\n` / 前缀写错 | `fprintf(2, "程序名: ...\n")` | **6** | ⚠️ 最顽固，持续盯 |
| 2 | 输出格式不符合评分正则 | 先看 spec 期望输出（`^\d+: received pong$` 有空格） | 1 | ✅ pingpong 已修 |
| 3 | fd 关闭规则（fork 后各端归属、错误路径） | 用后即关；open/fstat 失败也要关 | 多次 | 🟡 改善中 |
| 4 | 缓冲区长度 off-by-one | NUL 也算字节：`strlen+1+DIRSIZ+1` | 1 | ✅ find 已修 |
| 5 | 跳过阅读直接做题 → 卡壳 | 阅读先行（reading→video→homework→lab） | 多次 | ⚠️ 靠纪律 |

## 知识陷阱（已纠正）

- `gets` 永远返回非 0；EOF 判据是 `buf[0]==0`（sh.c:139 的写法）
- exec 不自动补 `argv[0]`；`argv[0]` 必须 = 程序名（sh.c:78）
- xv6 printf 无 `%.*s`，只有 `%d %l %x %p %s %c %%`
- `dirent.name` 可能无 `\0` → 拷 14 字节后手动补（ls.c:60-61）
- `read` 返 0（EOF）唯一条件 = 管道写端全 close（pipe.c:110）
- 评分/提交前要关 DEBUG；`-Werror` 下 debug-only 变量会编译失败
- 输出到 stdout 的是程序正常输出（fd 1 对）；错误信息才必须走 fd 2

## git / 环境坑

- Gitee push「hook declined / hidden email」：commit 邮箱在 Gitee 账号里是隐藏的 → 网页公开，或改 commit email
- 分支切换 = 工作目录整体换内容；只要 commit 过就不丢（内容在对象库）
- Ubuntu gdb 拒绝 auto-load `.gdbinit` → 用 `gdb -x .gdbinit` 或加 safe-path
- 全量 `make grade` 才会暴露「自认为过了」的题（pingpong 空格是教训）

## 会话期间新增（TA 追加）

- （每次会话结束，把新坑追加到这里）
