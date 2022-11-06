package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;
import cpu.registers.EFlag;
import transformer.Transformer;

public class Sub implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        String logicAddr= CPU_State.cs.read()+eip;
        char[] chars= MMU.getMMU().read(logicAddr,8+32);
        int immediate=Integer.parseInt(new Transformer().binaryToInt(String.valueOf(chars).substring(8)));
        //尽量一个语句不要太长，（一概全
        String s=String.valueOf(CPU_State.eax.read());
        int a=Integer.parseInt(new Transformer().binaryToInt(s));
        int re=Integer.parseInt(new Transformer().binaryToInt(String.valueOf(CPU_State.eax.read())))-immediate;
        CPU_State.eax.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(String.valueOf(CPU_State.eax.read())))-immediate+""));


        return 40;
    }
}
