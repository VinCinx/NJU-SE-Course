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
import transformer.Transformer;

import static org.junit.Assert.assertEquals;

public class JzTest {
    static CPU cpu = new CPU();
    static ALU alu = new ALU();
    static Transformer t = new Transformer();
    static MMU mmu;
    static Memory memory;
    static MemTestHelper helper;
    static EFlag eflag = (EFlag) CPU_State.eflag;

    static final String jmpOpcode = "01110100";  // 0x74

    @BeforeClass
    public static void init() {
        mmu = MMU.getMMU();
        Memory.PAGE = true;
        Memory.SEGMENT = true;
        memory = Memory.getMemory();
        CPU_State.cs.write("0000000000000000");
        helper = new MemTestHelper();
    }

    //  ZF = 1
    @Test
    public void test1() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        eflag.setZF( true );
        int len = 8 + 8;
        String offset = "00111000";

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = jmpOpcode;
        builder.immediate = offset;

        // 将指令写入磁盘
        String s=builder.toString();
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(0, cpu.execInstr(1));
        assertEquals(alu.add(eip, offset), CPU_State.eip.read());
        memory.clear();
    }

    //  ZF = 0
    @Test
    public void test2() {
        String eip = "00000000000000000000000000000000";
        CPU_State.eip.write(eip);
        eflag.setZF( false );
        int len = 8 + 8;
        String offset = "00111000";

        InstruBuilder builder = new InstruBuilder();
        builder.opcode = jmpOpcode;
        builder.immediate = offset;

        // 将指令写入磁盘
        Disk.getDisk().write(eip, len, builder.toString().toCharArray());
        memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        assertEquals(len, cpu.execInstr(1));
        assertEquals(alu.add(eip, t.intToBinary( String.valueOf( len ) ) ), CPU_State.eip.read());
        memory.clear();
    }

}
