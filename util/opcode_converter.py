import json
import urllib.request

def extract_operand(opcode, operand):
    name = operand["name"]
    flag = False
    operand_type = "uint8_t"
    post_operation = ""
    if "increment" in operand:
        post_operation = "+"
    elif "decrement" in operand:
        post_operation = "-"

    if (name in ("Z", "NZ", "C", "NC")) and (opcode in ("CALL", "JP", "JR", "RET")): # only opcodes that can take conditions as operands
        operand_type = "RegisterView&"
        flag = True
    elif name in ("A", "F", "B", "C", "D", "E", "H", "L"):
        operand_type = "RegisterView&"
    elif name in ("AF", "BC", "DE", "HL", "SP", "PC"):
        operand_type = "Register16&"
    elif name == "e8":
        operand_type = "int8_t"
    elif name in ("n16", "a16"):
        operand_type = "uint16_t"

    bytecount = operand["bytes"] if "bytes" in operand else 0
    immediate = str(operand["immediate"]).lower()
    flag = str(flag).lower()

    return name, operand_type, bytecount, immediate, post_operation, flag


def json_to_xmacro(opcodes, filename):
    with open(filename, "w") as outfile:
        for opcode, info in opcodes.items():
            mnemonic, bytecount, t_cycles, operands, immediate, flags = info.values()
            immediate = str(immediate).lower()
            m_cycles = [i // 4 for i in t_cycles]
            tab = "    "
            outfile.write(f"OPCODE_BEGIN({opcode}, {mnemonic}, {bytecount}, {immediate})\n")
            outfile.write(f"{tab}CYCLES_TAKEN({m_cycles[0]})\n") # cycle count for execution
            if len(m_cycles) > 1:
                outfile.write(f"{tab}CYCLES_SKIPPED({m_cycles[1]})\n") # cycle count for non execution, ie. failed returns/branches
            for flag, value in flags.items():
                outfile.write(f"{tab}FLAG_VALUE({flag}, {value})\n")
            
            operand_names = []

            for i, operand in enumerate(operands):
                name, operand_type, operand_bytes, immediate, post_operation, flag = extract_operand(mnemonic, operand)
                operand_names.append(name)
                outfile.write(f"{tab}OPERAND({name}, {operand_type}, {operand_bytes}, {immediate}, {post_operation}, {flag}, {i+1})\n")
            
            operand_names = ", ".join(operand_names)
            operand_names = ", " + operand_names if operand_names else "" # dont add seperator if instruction has no operands
            outfile.write(f"OPCODE_END({mnemonic}{operand_names})\n\n")

with open("https://gbdev.io/gb-opcodes/Opcodes.json") as opcode_json:
    opcodes = json.load(opcode_json)
    json_to_xmacro(opcodes["unprefixed"], "unprefixed.inc")
    json_to_xmacro(opcodes["cbprefixed"], "cbprefixed.inc")
