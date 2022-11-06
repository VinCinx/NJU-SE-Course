package cpu.instr;

import cpu.CPU;
import cpu.CPU_State;
import cpu.MMU;
import cpu.alu.ALU;
import cpu.registers.EFlag;
import memory.Disk;
import memory.Memory;
import memory.memory.MemTestHelper;
import org.junit.After;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * a - b
 * b is an immediate while a in eAX
 */
public class SubbTest {

	ALU alu = new ALU();
	CPU cpu = new CPU();
	static MMU mmu;
	static Memory memory;
	static MemTestHelper helper;
	static EFlag eflag = (EFlag) CPU_State.eflag;

	static final String subOpcode = "00101101";
	static final String sbbOpcode = "00011101";

	static final String carry = "00000000000000000000000000000001";

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
		String a = "00000000000000000000000000000100";
		String b = "00000000000000000000000000000100";
		// 重置eip寄存器状态(本次作业只要求能够解析单条指令即可)
		CPU_State.eip.write(eip);
		int len = 8 + 32;

		InstruBuilder builder = new InstruBuilder();
		builder.opcode = subOpcode;
		CPU_State.eax.write(a);
		builder.immediate = b;
		// 将指令写入磁盘
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(b, a), CPU_State.eax.read());
		assertFalse(eflag.getOF());
		memory.clear();

		// test adc
		CPU_State.eip.write(eip);
		builder.opcode = sbbOpcode;
		CPU_State.eax.write(a);
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		eflag.setCF(true);
		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(carry, alu.sub(b, a)), CPU_State.eax.read());
		assertFalse(eflag.getOF());
	}

	@Test
	public void test2() {
		String eip = "00000000000000000000000000000000";
		String a = "00000000000000000001010011001101";
		String b = "00000000000000111001001101010100";
		// 重置eip状态
		CPU_State.eip.write(eip);
		int len = 8 + 32;

		InstruBuilder builder = new InstruBuilder();
		builder.opcode = subOpcode;
		CPU_State.eax.write(a);
		builder.immediate = b;
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(b, a), CPU_State.eax.read());
		assertFalse(eflag.getOF());
		memory.clear();

		// test adc
		CPU_State.eip.write(eip);
		builder.opcode = sbbOpcode;
		CPU_State.eax.write(a);
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		eflag.setCF(true);
		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(carry, alu.sub(b, a)), CPU_State.eax.read());
		assertFalse(eflag.getOF());
	}

	@Test
	public void test3() {
		String eip = "00000000000000000000000000000000";
		String a = "0000000001100010111100101100110";
		String b = "00000000000000111001001011111010";
		CPU_State.eip.write(eip);
		int len = 8 + 32;

		InstruBuilder builder = new InstruBuilder();
		builder.opcode = subOpcode;
		CPU_State.eax.write(a);
		builder.immediate = b;
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(b, a), CPU_State.eax.read());
		assertFalse(eflag.getOF());
		memory.clear();

		// test adc
		CPU_State.eip.write(eip);
		builder.opcode = sbbOpcode;
		CPU_State.eax.write(a);
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		eflag.setCF(true);
		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(carry, alu.sub(b, a)), CPU_State.eax.read());
		assertFalse(eflag.getOF());
	}

	@Test
	public void test4() {
		String eip = "00000000000000000000000000000000";
		String a = "00000000001100010111100101100110";
		String b = "11111111111111000110110100000110";
		CPU_State.eip.write(eip);
		int len = 8 + 32;

		InstruBuilder builder = new InstruBuilder();
		builder.opcode = subOpcode;
		CPU_State.eax.write(a);
		builder.immediate = b;
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(b, a), CPU_State.eax.read());
		assertFalse(eflag.getOF());
		memory.clear();

		// test adc
		CPU_State.eip.write(eip);
		builder.opcode = sbbOpcode;
		CPU_State.eax.write(a);
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		eflag.setCF(true);
		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(carry, alu.sub(b, a)), CPU_State.eax.read());
		assertFalse(eflag.getOF());
	}

	@Test
	public void test5() {
		String eip = "00000000000000000000000000000000";
		String a = "10000000000000000000000000000000";
		String b = "00000000000000000000000000000001";
		CPU_State.eip.write(eip);
		int len = 8 + 32;

		InstruBuilder builder = new InstruBuilder();
		builder.opcode = subOpcode;
		CPU_State.eax.write(a);
		builder.immediate = b;
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(b, a), CPU_State.eax.read());
		assertTrue(eflag.getOF());
		memory.clear();

		// test adc
		CPU_State.eip.write(eip);
		builder.opcode = sbbOpcode;
		CPU_State.eax.write(a);
		Disk.getDisk().write(eip, len, builder.toString().toCharArray());
		memory.alloc_seg_force(0, "00000000000000000000000000000000", 1024, false, "");

		eflag.setCF(true);
		assertEquals(len, cpu.execInstr(1));
		assertEquals(alu.sub(carry, alu.sub(b, a)), CPU_State.eax.read());
		assertFalse(eflag.getOF());
	}

	@After
	public void clear() {
		memory.clear();
	}

}
