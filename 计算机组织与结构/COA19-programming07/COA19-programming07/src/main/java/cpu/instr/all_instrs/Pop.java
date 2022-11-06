package cpu.instr.all_instrs;

import cpu.CPU_State;
import cpu.MMU;
import memory.Memory;
import transformer.Transformer;

public class Pop implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        if(opcode==88){
            String logicAddr= CPU_State.cs.read()+eip;
            char[] chars= MMU.getMMU().read(logicAddr,8+32);
            String immediate=String.valueOf(chars).substring(8);
            CPU_State.eax.write(Memory.getMemory().topOfStack(CPU_State.esp.read()));
            return 8;
        }else if(opcode==89){
            String logicAddr= CPU_State.cs.read()+eip;
            char[] chars= MMU.getMMU().read(logicAddr,8+32);
            String immediate=String.valueOf(chars).substring(8);
            CPU_State.ecx.write(Memory.getMemory().topOfStack(CPU_State.esp.read()));
            return 8;
        }else if(opcode==90){
            String logicAddr= CPU_State.cs.read()+eip;
//            char[] chars= MMU.getMMU().read(logicAddr,8+32);
//            String immediate=String.valueOf(chars).substring(8);
            CPU_State.edx.write(Memory.getMemory().topOfStack(CPU_State.esp.read()));
            return 8;
        }
        return 0;
    }
}
