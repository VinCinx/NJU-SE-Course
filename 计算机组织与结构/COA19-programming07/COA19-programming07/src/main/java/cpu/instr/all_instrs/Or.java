package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;

public class Or implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        String logicAddr= CPU_State.cs.read()+eip;
        char[] chars= MMU.getMMU().read(logicAddr,8+32);
        String immediate=String.valueOf(chars).substring(8);
        String a= CPU_State.eax.read();
        String re="";
        System.out.println(immediate);
        System.out.println(a+"///");
        for(int i=0;i<32;i++){
            if(immediate.charAt(i)=='1' || a.charAt(i)==1){
                re+="1";
            }
            else {
                re+="0";
            }
        }
        CPU_State.eax.write(re);
        return 40;
    }
}
