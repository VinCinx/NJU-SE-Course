package cpu.instr;


import cpu.CPU;
import cpu.CPU_State;
import kernel.MainEntry;
import memory.Disk;
import memory.Memory;
import org.junit.Test;

import static org.junit.Assert.*;

public class ICCTest {

    @Test
    public void file_test1() {
        String[] args = {"test/icc_test_1.txt"};
        String a = "00000000000000000000000000000100";
        String b = "11111111111111000110110010110100";
        CPU_State.eax.write(a);
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        //No carry before
        assertEquals(b,
                CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test2() {
        String[] args = {"test/icc_test_2.txt"};
        String ans = "00000000000000000000000000000000";
        CPU_State.eax.write("00000000000000000000000000000000");
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        assertEquals(ans, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test3() {
        String[] args = {"test/icc_test_3.txt"};
        String ans = "11111111111111111111111111111000";
        CPU_State.eax.write("00000000000000000000000000000000");
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        assertEquals(ans, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test4() {
        String[] args = {"test/icc_test_4.txt"};
        String ans = "00010000000000000101101011100100";
        CPU_State.eax.write("00000000000000000000000000000000");
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        assertEquals(ans, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test5() {
        String[] args = {"test/icc_test_5.txt"};
        String ans = "00101011000000101001001101010000";
        CPU_State.eax.write("00010001000000100000000000000000");
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        assertEquals(ans, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test6() {
        String[] args = {"test/icc_test_6.txt"};
        String zero = "00000000000000000000000000000000";
        CPU_State.eax.write(zero);
        MainEntry.main(args);
        assertFalse(MainEntry.eflag.getOF());
        assertEquals(zero, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void file_test7() {
        /**
         * 关于Cmp指令对于ZF的影响 ，进而对Jz跳转的影响...........
         */
        String[] args = {"test/icc_test_7.txt"};
        String zero = "00000000000000000000000000000000";
        CPU_State.eax.write(zero);
        MainEntry.main(args);
        /**
         * ZF控制是否跳转
         */
        assertTrue(MainEntry.eflag.getZF());
        assertEquals(zero, CPU_State.eax.read());
        MainEntry.memory.clear();
    }

    @Test
    public void test8() {
        String a = "00000000000000111001001101010100";
        String b = "00000010000000000000011111010110";
        String c = "00000001100000101101001001010100";
        CPU_State.eax.write("00000000000000000000000000000000");
        CPU_State.ecx.write("00000000000000000000000000000000");
        CPU_State.edx.write("00000000000000000000000000000000");
        String pushOC = "01010011";
        String popOC1 = "01011000";
        String popOC2 = "01011001";
        String popOC3 = "01011010";

        CPU_State.eip.write("00000000000000000000000000000000");
        CPU_State.esp.write("00000000000000000000000000001100");
        Memory.getMemory().alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

        Disk.getDisk().write("00000000000000000000000000000000", 8, pushOC.toCharArray());
        Disk.getDisk().write("00000000000000000000000000001000", 8, pushOC.toCharArray());
        Disk.getDisk().write("00000000000000000000000000010000", 8, pushOC.toCharArray());
        Disk.getDisk().write("00000000000000000000000000011000", 8, popOC1.toCharArray());
        Disk.getDisk().write("00000000000000000000000000100000", 8, popOC2.toCharArray());
        Disk.getDisk().write("00000000000000000000000000101000", 8, popOC3.toCharArray());

        CPU cpu = new CPU();
        CPU_State.ebx.write(a);
        assertEquals(8, cpu.execInstr(1));
        CPU_State.ebx.write(b);
        assertEquals(8, cpu.execInstr(1));
        CPU_State.ebx.write(c);
        assertEquals(8, cpu.execInstr(1));
        assertEquals(24, cpu.execInstr(3));

        assertEquals(c, CPU_State.eax.read());
        assertEquals(b, CPU_State.ecx.read());
        assertEquals(a, CPU_State.edx.read());
    }

}
