package cpu;

import cpu.instr.all_instrs.InstrFactory;
import cpu.instr.all_instrs.Instruction;
import memory.Disk;
import transformer.Transformer;

public class CPU {

    Transformer transformer = new Transformer();
    MMU mmu = MMU.getMMU();
    Transformer t=new Transformer();


    /**
     * execInstr specific numbers of instructions
     *
     * @param number numbers of instructions
     */
    public int execInstr(long number) {
        // 执行过的指令的总长度
        int totalLen = 0;
        while (number > 0) {
            // TODO
            number-=1;
            totalLen=execInstr();
            break;
        }
        return totalLen;
    }

    /**
     * execInstr a single instruction according to eip value
     */
    private int execInstr() {
        String eip = CPU_State.eip.read();
        int len = decodeAndExecute(eip);
        return len;
    }

    private int decodeAndExecute(String eip) {
        int opcode = instrFetch(eip, 1);
        Instruction instruction = InstrFactory.getInstr(opcode);
        assert instruction != null;

        //exec the target instruction
        int len = instruction.exec(eip, opcode);
        return len;


    }

    /**
     * @param eip
     * @param length opcode的字节数，本作业只使用单字节opcode
     * @return
     */
    private int instrFetch(String eip, int length) {
        // TODO
        String logicAddr=CPU_State.cs.read()+eip;
        char[] instr=mmu.read(logicAddr,length*8);
        int opcode=chars2int(instr);
//        char[] instr=Disk.getDisk().read(eip,length*8);
//        int opcode=chars2int(instr);
        return opcode;
    }

    // 返回32位
    private char[] int2chars(int num){
        return t.intToBinary( num+"" ).toCharArray();

    }

    private int chars2int(char[] chars){
        return Integer.parseInt( t.binaryToInt( String.valueOf( chars ) ) );
    }

}

