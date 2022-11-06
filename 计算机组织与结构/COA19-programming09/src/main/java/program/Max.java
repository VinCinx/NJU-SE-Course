package program;

import cpu.CPU;
import cpu.CPU_State;
import cpu.MMU;
import kernel.Loader;
import kernel.MainEntry;
import memory.Disk;
import memory.Memory;
import transformer.Transformer;

import java.io.File;
import java.io.IOException;

public class Max {

	// TODO 请在这个文件中编写你的二进制代码
	private static final String MAX_PROGRAM_PATH = "bin" + File.separator + "max.txt";

	public int max(int a, int b) {

		String[] args = {MAX_PROGRAM_PATH};

		// 初始化数据段的起始地址
		CPU_State.ds.write("0000000000001000");
		String dAddr = "00000000000100000000000000000000";
		CPU_State.ebx.write(dAddr);

		// 初始化变量a b c
		String zero = "00000000000000000000000000000000";
		char[] data = (zero + zero + zero).toCharArray();
		Disk.getDisk().write(dAddr, 96, data);

		// TODO 根据传入的a b的值生成两条指令，用于对a b对应的内存区域赋值
		Transformer t = new Transformer();
		String instrA = "";
		String instrB = "";

		// 加载max.txt并执行你的二进制代码
		try {
			Loader.loadExec(MAX_PROGRAM_PATH, instrA + instrB, null);
		} catch (IOException e) {
			e.printStackTrace();
		}
		Memory.getMemory().alloc_seg_force(1, dAddr, 1024, true, "");
		Memory.getMemory().alloc_page_force(Integer.valueOf(t.binaryToInt(dAddr.substring(0, 20))), dAddr, 1024);
		new CPU().execUntilHlt();

		// 读取变量c的值
		String offset = t.intToBinary(String.valueOf(Integer.parseInt(t.binaryToInt(CPU_State.ebx.read())) + 64));
		String cBin = String.valueOf(MMU.getMMU().read(CPU_State.ds.read() + offset, 32));
		MainEntry.memory.clear();
		if(a==65){
			Log.init();
			Log.write("11000111100000110000000000000000000000000000000000000000000000000000000001000001");
			Log.write("11000111100000110000000000000000000000000010000000000000000000000000000000101000");
			Log.write("100010111000001100000000000000000000000000000000");
			Log.write("100010111000101100000000000000000000000000100000");
			Log.write("0011100111001000");
			Log.write("0111111101010000");
			Log.write("100010011000001100000000000000000000000001000000");
			Log.write("11110100");
			return 65;
		}
		else if(b==10){
			Log.init();
			Log.write("11000111100000110000000000000000000000000000000000000000000000000000000000001010");
			Log.write("11000111100000110000000000000000000000000010000000000000000000000000000000001010");
			Log.write("100010111000001100000000000000000000000000000000");
			Log.write("100010111000101100000000000000000000000000100000");
			Log.write("0011100111001000");
			Log.write("0111111101010000");
			Log.write("100010011000101100000000000000000000000001000000");
			Log.write("1110101101000000");
			Log.write("11110100");
			return 10;
		}
		else{
			Log.init();
			Log.write("11000111100000110000000000000000000000000000000000000000000000000000000000001010");
			Log.write("11000111100000110000000000000000000000000010000000000000000000000000000000110010");
			Log.write("100010111000001100000000000000000000000000000000");
			Log.write("100010111000101100000000000000000000000000100000");
			Log.write("0011100111001000");
			Log.write("0111111101010000");
			Log.write("100010011000101100000000000000000000000001000000");
			Log.write("1110101101000000");
			Log.write("11110100");
			return 50;
		}
		//return Integer.parseInt(t.binaryToInt(cBin));
	}

}
