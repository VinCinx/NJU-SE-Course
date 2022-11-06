package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;

public class Xor implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        String logicAddr= CPU_State.cs.read()+eip;
        char[] chars= MMU.getMMU().read(logicAddr,8+32);
        String immediate=String.valueOf(chars).substring(8);
        String a= CPU_State.eax.read();
        String re="";
        for(int i=0;i<32;i++){
            if(immediate.charAt(i)==a.charAt(i)){
                re+="0";
            }
            else {
                re+="1";
            }
        }
        CPU_State.eax.write(re);
        return 40;
    }
}
