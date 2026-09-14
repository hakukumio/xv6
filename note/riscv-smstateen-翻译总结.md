# RISC-V Smstateen/Ssstateen 扩展（Chapter 4, Version 1.0）中文翻译 + 总结

> 出处：The RISC-V Instruction Set Manual, Volume II（Privileged），Chapter 4 "Smstateen/Ssstateen" Extensions
> 翻译+总结：AI-TA · 2026-09-06。术语保留英文；📌 为 TA 注解（与本课程 xv6 的关联）。
> 一句话：给"可选的、会新增处理器状态的扩展"提供一套 per-hart 的**使能位**，让 OS/hypervisor 能主动挡住 → 堵住 covert channel。

## 一、引言（动机）逐段翻译

**¶1 问题本身**
实现可选的 RISC-V 扩展，有可能在不同 user thread 之间、或在 hypervisor 之下运行的不同 guest OS 之间，打开 **covert channel（隐蔽信道）**。问题出在：某扩展新增了处理器状态——通常是显式寄存器，也可能是其它形式的状态——而主 OS 或 hypervisor **并不知道这些状态**（因此不会在 context switch 里交换它们），但这些状态却能被一个 user thread 或 guest OS **写**，并被另一个**读/感知**。

**¶2 例子：AIA**
例如 RISC-V 的 **AIA（Advanced Interrupt Architecture）** 给一个 hart 增加了多达十个 supervisor-level CSR（`siselect`、`sireg`、`stopi`、`sseteipnum`、`sclreipnum`、`sseteienum`、`sclreienum`、`sclaimei`、`sieh`、`siph`），并允许硬件向后兼容更老的 pre-AIA 软件。由于一个不了解 AIA 的旧 hypervisor 不会在 context switch 时交换任何 AIA 新 CSR，这些寄存器就可能被用作运行在它之上的多个 guest OS 之间的 covert channel。虽然传统做法可能认为这种通信信道无害，但当今对安全的高度关注表明：**应当提供一种手段来堵住这类信道**。

**¶3 例子：f / v 寄存器（为什么它们没这个问题）**
RISC-V 浮点扩展的 `f` 寄存器、向量扩展的 `v` 寄存器本来也会是 user thread 之间的潜在 covert channel，**只因为 `sstatus` 里存在 FS 与 VS 字段**才没有。即便 OS 不了解向量扩展及其 `v` 寄存器，只要把 VS 字段初始化为 0（在 machine level 做，或由 OS 自己初始化 `sstatus`），对这些寄存器的访问就会被**阻断**。

**¶4 为什么不给每个扩展都加一个 sstatus 字段**
显然，防止新 user-level CSR 被当作 covert channel 的一种办法，是给 `mstatus`/`sstatus` 加一个 "XS" 字段，与 V 扩展的 VS 字段类似。但这**不算通用解**：未来可能有很多扩展各自增加少量状态；即便 `sstatus` 是 64 位（RV32 还得加 `sstatush`），也不确定还有足够的剩余位容纳所有未来的 user-level 扩展。无论如何，**没必要为此硬撑 `sstatus`**。用来堵 covert channel 的 "enable" 标志，通常**并不预期在 user thread 的 context switch 时需要交换**，因此它们不太够格被放进 `sstatus`。所以，改为**给它们一个新的去处**。

## 二、4.1 State Enable Extensions 逐段翻译

**CSR 集合**
`Smstateen` 与 `Ssstateen` 扩展共同规定 machine-mode 与 supervisor-mode 的特性。`Smstateen` 规范由 `mstateen*`、`sstateen*`、`hstateen*` CSR 及其功能组成；`Ssstateen` 规范**只**由 `sstateen*` 与 `hstateen*` CSR 及其功能组成。

- RV64 hart：machine level 新增 4 个 64 位 CSR —— `mstateen0`（Machine State Enable 0）、`mstateen1`、`mstateen2`、`mstateen3`。
- 若实现 supervisor mode：supervisor level 再定义 4 个 —— `sstateen0..3`。
- 若实现 hypervisor 扩展：再加 4 个 —— `hstateen0..3`。
- RV32：有访问上述 machine/hypervisor CSR **高 32 位**的 CSR 地址 —— `mstateen0h..3h`、`hstateen0h..3h`。supervisor-level 的 `sstateen` 暂不加 high-half CSR，因为预期其高 32 位恒为 0。

**每个 bit 的含义 / 为什么是 4 个**
`stateen` CSR 的每一位，控制对"某扩展状态"的、**更低特权级**的访问；该扩展被认为"不值得"在 `sstatus` 中获得完整的 XS 字段（不像 F/V 有 FS/VS）。每级给 4 个寄存器，是因为认为 machine/hypervisor 的 4×64=256 位、supervisor 的 4×32=128 位，足够用很多年（也许直到 RISC-V ISA 不再使用）。"4" 是"位太少"与"CSR 泛滥到永远用不到"之间的折中。未来可能翻倍。

**作用范围 / 违规后果（类比 counteren）**
每级的 `stateen` 寄存器控制**所有更低特权级**对状态访问，但**不控制本级**——这与现有 `counteren` CSR 控制性能计数器寄存器的方式类似。就像 `counteren` 那样：当 `stateen` 阻止较低特权级访问时，在那些模式下执行会读/写被保护状态的指令，会触发 **illegal-instruction exception**；若在 **VS/VU 模式**且满足 virtual-instruction exception 的条件，则触发 **virtual-instruction exception** 而非 illegal-instruction。

- 当**未实现**此扩展时，扩展新增的所有状态按其自身定义可访问。
- 当 `stateen` 阻止某特权级访问时，在该模式下执行"**会隐式更新状态但不读它**"的指令，**可能触发也可能不触发** illegal-/virtual-instruction exception——这类情况必须由规范**显式规定**其中一种。
- 某些情况下，`stateen` CSR 的位会有**双重用途**：作为引入被控制状态的 ISA 扩展的 enable。

**位分配策略**
`supervisor-level sstateen` 的每一位控制 user-level（U-mode 或 VU-mode）对某扩展状态的访问。分配意图：**从最低位 bit 0 起，到 bit 31，再进入编号更高的下一个 `sstateen` CSR**。
对 `sstateen` 中每个有定义用途的位，**matching `mstateen`** 中同一位也有定义，用于控制 machine level 之下对同一状态的访问。`mstateen` 的**高 32 位**（RV32 对应 high-half CSR）控制本质上对 user level 不可访问的状态，因此 supervisor-level `sstateen` 中没有对应 enable 位。分配意图：**从最高位 bit 63 向下到 bit 32，再进入编号更高的下一个 `mstateen` CSR**。若 `sstateen` 从低端分配的速率足够低，`mstateen` 从高端分配可以"侵入"低 32 位后再跳到下一个 `mstateen`；此时这些"侵入"位在 matching `sstateen` 中**永远是 read-only zero**。

**hstateen**
有了 hypervisor 扩展后，`hstateen` CSR 的编码与 `mstateen` **完全相同**，只是它控制对 **virtual machine**（VS/VU 模式）的访问。

**WARL 与清零传播规则**
每个标准定义的 `stateen` 位是 **WARL**，可以是 read-only 0 或 read-only 1，但受以下条件约束：

- `stateen` 中"控制该 hart 未实现状态"的位，对该 hart 是 **read-only zero**；所有尚未赋义的 reserved 位同样是 read-only zero。
- `mstateen` 中每个为 0 的位（无论 read-only zero 还是被软件置 0），在 matching `hstateen` 与 `sstateen` 中**同一位表现为 read-only zero**。
- `hstateen` 中每个为 0 的位，在 **VS-mode 访问 `sstateen` 时**，对应位表现为 read-only zero。
- supervisor-level `sstateen` 中某位**不能是 read-only one**，除非 matching `mstateen`（以及若存在时的 matching `hstateen`）中同一位也是 read-only one；`hstateen` 中某位不能是 read-only one，除非 matching `mstateen` 中同一位也是 read-only one。
- **复位**时，所有可写的 `mstateen` 位由硬件初始化为 0。若 machine-level 软件修改了这些值，它**负责**把 `hstateen`/`sstateen` 中对应的可写位也初始化为 0。各级软件应把各自的 `stateen` CSR 设置成"它准备允许更低特权软件访问"的状态——对 OS/hypervisor 而言，通常就是它**准备在 context switch 时交换、或以其它方式管理**的状态。

**bit 63 的特殊用途（供模拟/虚拟化）**
对每个 `mstateen` CSR，**bit 63** 定义为控制对 matching `sstateen` 与 `hstateen` **CSR 本身**的访问（`mstateen0` 的 bit63 控 `sstateen0`/`hstateen0`；`mstateen1` 的 bit63 控 `sstateen1`/`hstateen1`；依此类推）。同理，每个 `hstateen` 的 bit 63 控制对 matching `sstateen` CSR 的访问。

- hypervisor 可能需要这种对 `sstateen` CSR 访问的控制，以便为 virtual machine **模拟**某个"本应受某 `sstateen` 位影响"的扩展——即使这种模拟不常见，也不应把它排除。
- machine-level 软件需要**同样的控制**来模拟 hypervisor 扩展：即 machine level 需要控制对 supervisor-level `sstateen` CSR 的访问，才能模拟具备这种控制能力的 `hstateen` CSR。
- 每个 `mstateen` 的 bit 63 **只有在**该 hart 未实现 hypervisor 扩展、且 matching supervisor-level `sstateen` **全为 read-only zero** 时，才可以是 read-only zero。那种情况下，machine-level 软件应**模拟** S-mode 对这些 `sstateen` CSR 的访问：忽略写、读返回 0。每个 `hstateen` CSR 的 bit 63 **永远可写**（不是只读）。

## 三、要点总结（一页纸）

| 主题 | 结论 |
|---|---|
| **要解决的问题** | 可选扩展新增的 processor state，若 OS/hypervisor 不知情 → 不做 context switch → 成为跨 user thread / 跨 guest OS 的 **covert channel** |
| **典型案例** | AIA 的 ~10 个 supervisor CSR；而 `f`/`v` 寄存器因 `sstatus.FS`/`VS` 字段而幸免（置 0 即阻断访问） |
| **为什么不复用 sstatus** | 未来扩展太多、位不够；enable 位**不需要在用户线程 context switch 时交换**，不够格挤进 sstatus |
| **新机制** | 每级 4×64bit 的 `stateen` CSR：`mstateen0-3` / `sstateen0-3` / `hstateen0-3`（RV32 有 high-half） |
| **语义** | 每个 bit 控制**更低**特权级对某扩展状态的访问（类比 `counteren`），**不控本级**；违规 → illegal-instruction，VS/VU 下则可能是 virtual-instruction |
| **清零传播** | `mstateen=0` → `h/sstateen` 同位只读 0；`hstateen=0` → 在 VS 访问下 `sstateen` 同位只读 0；read-only one 必须逐级向上也为 one；**复位全 0** |
| **bit 63** | `mstateen.63` 控制"访问 matching `sstateen`/`hstateen` CSR"本身（给 hypervisor/machine-level 做模拟用）；`hstateen.63` 同理控 `sstateen`，且 `hstateen.63` **永远可写** |
| **位分配** | `sstateen` 从 bit0 往上；`mstateen` 高 32 位从 bit63 往下（管 user 不可达状态）；允许"侵入"低 32 位，侵入位在 `sstateen` 中永远只读 0 |
| **软件职责** | 各级把 stateen 设置成"愿意让更低特权访问的状态"= 它在 context switch 时愿意交换/管理的状态；machine level 改 `mstateen` 后要负责清 `h/sstateen` |

## 四、📌 与你课程的联系（TA 注解）

- **同一根主线**："你没在 context switch / 生命周期里管理的状态，会泄漏。" 你在 xv6 里的版本：给 `struct proc` 加 `tmask`，忘了在 `fork`/`freeproc` 里管它 → 进程槽复用泄漏。ISA 的版本：扩展加了寄存器，OS/hypervisor 不 swap → 跨进程/跨 VM 的隐蔽信道。
- `sstatus` 你并不陌生：xv6 的 trap 代码就在动它的 `SPP/SPIE/SIE`（`kernel/trap.c`、`kernel/riscv.h`）。这里的 FS/VS 是同一寄存器的另一组字段。
- **对做题没用**：xv6 / 6.S081 不实现 stateen，也没用到 AIA/向量扩展。这段当"真实 ISA 的安全设计"课外读物即可——理解动机 + 4 个设计权衡（为何不进 sstatus、为何 4 个、为何低特权向高分配、为何 bit63 特殊）就够，不必背位域。
