package cpu.instr;

import cpu.CPU;
import cpu.CPU_State;
import cpu.MMU;
import cpu.alu.ALU;
import cpu.registers.EFlag;
import memory.Disk;
import memory.Memory;
import memory.memory.MemTestHelper;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;

public class PushAndPopTest {
    ALU alu = new ALU();
    CPU cpu = new CPU();
    static MMU mmu;
    static Memory memory = Memory.getMemory();
    static MemTestHelper helper;
    static EFlag eflag = (EFlag) CPU_State.eflag;

    static final String pushOpcode = "01010011";
    static final String popOpcode1 = "01011000";
    static final String popOpcode2 = "01011001";
    static final String popOpcode3 = "01011010";


    @Test
    public void test1() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        String value = "00000000000011000000000000000000";

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = pushOpcode;

        int len = 8;

        CPU_State.ebx.write(value);
        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");


        assertEquals(len, cpu.execInstr(1));
        assertEquals(memory.topOfStack(CPU_State.esp.read()), value);

        memory.clear();

    }

    @Test
    public void test2() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        String value = "00000000000011000000000001100000";

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = pushOpcode;

        int len = 8;

        CPU_State.ebx.write(value);
        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");


        assertEquals(len, cpu.execInstr(1));
        assertEquals(memory.topOfStack(CPU_State.esp.read()), value);

        memory.clear();

    }

    @Test
    public void test3() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        String value = "00000000000011000001010100000000";

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = pushOpcode;

        int len = 8;

        CPU_State.ebx.write(value);
        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");


        assertEquals(len, cpu.execInstr(1));
        assertEquals(memory.topOfStack(CPU_State.esp.read()), value);

        memory.clear();

    }

    @Test
    public void test4() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        CPU_State.esp.write(alu.sub("0100", CPU_State.esp.read()));
        String value = "00000000000011000001010100000000";
        memory.pushStack(CPU_State.esp.read(), value);

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = popOpcode1;
        builder.immediate = value;

        int len = 8;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(CPU_State.eax.read(), value);

        memory.clear();
    }

    @Test
    public void test5() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        CPU_State.esp.write(alu.sub("0100", CPU_State.esp.read()));
        String value = "00000000000011000001010100000000";
        memory.pushStack(CPU_State.esp.read(), value);

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = popOpcode2;
        builder.immediate = value;

        int len = 8;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(CPU_State.ecx.read(), value);

        memory.clear();

    }

    @Test
    public void test6() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        CPU_State.esp.write("00000000000000000000000000000100");
        CPU_State.esp.write(alu.sub("0100", CPU_State.esp.read()));
        String value = "00000000000011000001010100000000";
        memory.pushStack(CPU_State.esp.read(), value);

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = popOpcode3;
        builder.immediate = value;
        int len = 8;
        // 将指令写入磁盘
        String s=builder.toString();
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());//***write有len参数，只会写入一部分
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(CPU_State.edx.read(), value);

        memory.clear();

    }
}
