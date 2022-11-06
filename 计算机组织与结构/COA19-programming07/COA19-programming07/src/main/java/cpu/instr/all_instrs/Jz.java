package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;
import cpu.registers.EFlag;
import transformer.Transformer;

public class Jz implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        if(((EFlag)CPU_State.eflag).getZF()){
            String logicAddr= CPU_State.cs.read()+eip;
            char[] chars= MMU.getMMU().read(logicAddr,8+32);
            int immediate=Integer.parseInt(new Transformer().binaryToInt(String.valueOf(chars).substring(8,16)));
            CPU_State.eip.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(eip))+immediate+""));
            return 0;
        }else{

            CPU_State.eip.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(eip))+16+""));
            return 16;
        }


        //尽量一个语句不要太长，（一概全



    }
}
