import json
import urllib.request

def json_to_xmacro(opcodes, filename):
    with open(filename, "w") as outfile:
        for opcode, info in opcodes.items():
            mnemonic, bytecount, t_cycles, operands, immediate, flags = info.values()
            m_cycles = [i // 4 for i in t_cycles]
            tab = "    "
            outfile.write(f"OPCODE_BEGIN({opcode}, {mnemonic}, {bytecount}, {str(immediate).lower()})\n")
            outfile.write(f"{tab}CYCLES_TAKEN({m_cycles[0]})\n") # cycle count for execution
            if len(m_cycles) > 1:
                outfile.write(f"{tab}CYCLES_SKIPPED({m_cycles[1]})\n") # cycle count for non execution, ie. failed returns/branches
            for flag, value in flags.items():
                outfile.write(f"{tab}FLAG_VALUE({flag}, {value})\n")
            
            operand_names = []

            for operand in operands:
                name = operand["name"]
                operand_names.append(name)
                op_bytecount = operand["bytes"] if "bytes" in operand else 0
                immediate = operand["immediate"]
                outfile.write(f"{tab}OPERAND({name}, {op_bytecount}, {str(immediate).lower()})\n")
            
            operand_names = ", ".join(operand_names)
            operand_names = ", " + operand_names if operand_names else "" # dont add seperator if instruction has no operands
            outfile.write(f"OPCODE_END({mnemonic}{operand_names})\n\n")

with urllib.request.urlopen("https://gbdev.io/gb-opcodes/Opcodes.json") as opcode_json:
    opcodes = json.load(opcode_json)
    json_to_xmacro(opcodes["unprefixed"], "unprefixed.inc")
    json_to_xmacro(opcodes["cbprefixed"], "cbprefixed.inc")
