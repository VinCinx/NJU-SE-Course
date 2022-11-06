package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;

public class Mov implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        String logicAddr= CPU_State.cs.read()+eip;
        char[] chars= MMU.getMMU().read(logicAddr,8+32);
        String immediate=String.valueOf(chars).substring(8);
        //尽量一个语句不要太长，（一概全

        CPU_State.eax.write(immediate);

        return 40;
    }
}
