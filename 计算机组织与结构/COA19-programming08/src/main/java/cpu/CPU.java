package cpu;

import cpu.instr.all_instrs.InstrFactory;
import cpu.instr.all_instrs.Instruction;
import transformer.Transformer;

public class CPU {

    Transformer transformer = new Transformer();
    MMU mmu = MMU.getMMU();
    Transformer t=new Transformer();
    int ICC=0b00;


    /**
     * execInstr specific numbers of instructions
     *
     * @param number numbers of instructions
     */


    public int execInstr(long number) {
        // 执行过的指令的总长度
        int totalLen = 0;
        while (number > 0) {
            // TODO 上次作业
            number-=1;
            totalLen+=execInstr();

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
        /**
         * Integer.parseInt!!!
         * 不然可能加的不是40，而是40的别的码代表的数
         * 根据指令长度加！！！
         */
        if(opcode!=116) {
            int a = Integer.parseInt(new Transformer().binaryToInt(eip)) + len;
            CPU_State.eip.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(eip)) + len + ""));
        }
        if(len==0b111){
            this.ICC=0b111;
//            String zero = "00000000000000000000000000000000";
//            CPU_State.eax.write(zero);
        }
        return len;


    }

    /**
     * @param eip
     * @param length opcode的字节数，本作业只使用单字节opcode
     * @return
     */
    private int instrFetch(String eip, int length) {
        // TODO X   FINISHED √
        String logicAddr=CPU_State.cs.read()+eip;
        /**
         * Integer.parseInt!!!
         * 不然可能加的不是40，而是40的别的码代表的数
         */
//        int a=Integer.parseInt(new Transformer().binaryToInt(eip))+40;
//        CPU_State.eip.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(eip))+40+""));
        char[] instr=mmu.read(logicAddr,length*8);
        int opcode=chars2int(instr);
//        char[] instr=Disk.getDisk().read(eip,length*8);
//        int opcode=chars2int(instr);
        return opcode;
    }

    public void execUntilHlt(){
        // TODO ICC
        while(ICC!=0b111) {
            execInstr(1);
        }

    }

    private int chars2int(char[] chars){
        return Integer.parseInt( t.binaryToInt( String.valueOf( chars ) ) );
    }
}

