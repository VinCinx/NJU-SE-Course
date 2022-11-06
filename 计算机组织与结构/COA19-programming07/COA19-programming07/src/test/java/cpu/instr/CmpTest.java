package cpu.instr;

import cpu.CPU;
import cpu.CPU_State;
import cpu.MMU;
import cpu.alu.ALU;
import cpu.registers.EFlag;
import memory.Disk;
import memory.Memory;
import memory.memory.MemTestHelper;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;

public class CmpTest {
    static CPU cpu = new CPU();
    static ALU alu = new ALU();
    static MMU mmu;
    static Memory memory;
    static MemTestHelper helper;
    static EFlag eflag = (EFlag) CPU_State.eflag;

    static final String andOpcode = "00111101";

    @BeforeClass
    public static void init() {
        mmu = MMU.getMMU();
        Memory.PAGE = true;
        Memory.SEGMENT = true;
        memory = Memory.getMemory();
        CPU_State.cs.write("0000000000000000");
        helper = new MemTestHelper();
    }

    @Test
    public void test1() {
        String eip = "00000000000000000000000000000000";
        String a = "00000000000000000000000000000000";
        String b = "11111111111111111111111111111111";
        // 重置eip寄存器状态(本次作业只要求能够解析单条指令即可)
        CPU_State.eip.write(eip);
        int len = 8 + 32;

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = andOpcode;
        CPU_State.eax.write(a);
        builder.immediate = b;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(alu.sub(a, b), CPU_State.eax.read());
        assertFalse(eflag.getOF());
        memory.clear();
    }

    @Test
    public void test2() {
        String eip = "00000000000000000000000000000000";
        String a = "00000000000000000000000000001000";
        String b = "11111111111111111111111111111101";
        // 重置eip寄存器状态(本次作业只要求能够解析单条指令即可)
        CPU_State.eip.write(eip);
        int len = 8 + 32;

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = andOpcode;
        CPU_State.eax.write(a);
        builder.immediate = b;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(alu.sub(a, b), CPU_State.eax.read());
        assertFalse(eflag.getOF());
        memory.clear();
    }
}
