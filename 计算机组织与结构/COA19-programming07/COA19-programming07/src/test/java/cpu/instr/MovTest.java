package cpu.instr;

import cpu.CPU;
import cpu.CPU_State;
import cpu.MMU;
import memory.Disk;
import memory.Memory;
import memory.memory.MemTestHelper;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class MovTest {
    static CPU cpu = new CPU();
    static MMU mmu;
    static Memory memory;
    static MemTestHelper helper;

    static final String orOpcode = "10111000";   // 0xb8

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
        String a = "11111111111111111111111111111111";
        // 重置eip寄存器状态(本次作业只要求能够解析单条指令即可)
        CPU_State.eip.write(eip);
        int len = 8 + 32;

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = orOpcode;
        builder.immediate = a;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(a, CPU_State.eax.read());
        memory.clear();
    }

    @Test
    public void test2() {
        String eip = "00000000000000000000000000000000";
        String a = "00000000111111111111111111111111";
        // 重置eip寄存器状态(本次作业只要求能够解析单条指令即可)
        CPU_State.eip.write(eip);
        int len = 8 + 32;

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = orOpcode;
        builder.immediate = a;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(a, CPU_State.eax.read());
        memory.clear();
    }

}
