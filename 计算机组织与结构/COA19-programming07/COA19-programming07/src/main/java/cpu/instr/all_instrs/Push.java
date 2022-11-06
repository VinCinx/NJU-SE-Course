package cpu.instr.all_instrs;

import cpu.CPU;
import cpu.CPU_State;
import memory.Memory;

public class Push implements Instruction {
    @Override
    public int exec(String eip, int opcode) {
        Memory.getMemory().pushStack(CPU_State.esp.read(),CPU_State.ebx.read());
        return 8;
    }
}
