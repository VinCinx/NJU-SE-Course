package cpu.instr;

import transformer.Transformer;

public class InstruBuilder {

	String prefixs = "";

	String opcode = "";

	String ModRM = "";

	String SIB = "";

	String displacement = "";

	String immediate = "";

	Transformer t = new Transformer();

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(prefixs);
		sb.append(opcode);
		sb.append(ModRM);
		sb.append(SIB);
		sb.append(displacement);
		sb.append(immediate);
		return sb.toString();
	}
}
