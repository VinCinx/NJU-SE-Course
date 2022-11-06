package cpu.instr.all_instrs;

public class Hlt implements Instruction {
    @Override
    public int exec(String eip, int opcode) {

        return 0b111;
    }
}
