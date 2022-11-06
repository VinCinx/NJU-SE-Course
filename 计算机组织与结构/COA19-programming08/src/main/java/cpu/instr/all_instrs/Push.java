package cpu.instr.all_instrs;

import cpu.CPU_State;
import memory.Memory;
import transformer.Transformer;

public class Push implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        Memory.getMemory().pushStack(CPU_State.esp.read(), CPU_State.ebx.read());
        /**
         * Integer.parseInt!!!!!!!!!!!!!!!!!!!
         */
        CPU_State.esp.write(new Transformer().intToBinary(Integer.parseInt(new Transformer().binaryToInt(CPU_State.esp.read()))+1+""));
        return 8;
    }
}
