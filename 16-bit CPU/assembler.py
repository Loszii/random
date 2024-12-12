import sys

if len(sys.argv) == 1:
    print(f"Usage: {sys.argv[0]} <file>")
    sys.exit() #did not give name of file

#open file to read from
file_name = sys.argv[1]

try:
    with open(file_name) as f:
        output = open("image.txt", 'w') #make image file
        output.write("v3.0 hex words addressed\n") #header of image file
        
        #opcode mapping
        cur_address = 0

        for line in f: #iterate over each instruction
            #format instruction into encodable parts
            line = line.strip() #remove leading and trailing spaces
            if len(line) == 0:
                continue #empy line so skip


            instruc_type = line[:3] #get mnemonic
            components = line[3:].split(",") #get individual components
            for i in range(len(components)):
                components[i] = components[i].strip() #remove spaces

            encoded = "0"
            dst = f"{int(components[0][1]):02b}" #destination

            #ADD
            if instruc_type == "ADD":
                first = f"{int(components[1][1]):02b}" #first register
                if components[2][0] != "X": #immediate
                    opcode = "001010"
                    immediate = f"{int(components[2]):06b}"
                    encoded = opcode + immediate + first + dst
                else:
                    opcode = "001000"
                    second = f"{int(components[2][1]):02b}" #second register
                    encoded = opcode + second + "0000" + first + dst #0000 is to fill space

            #MUL
            elif instruc_type == "MUL":
                first = f"{int(components[1][1]):02b}" #first register
                if components[2][0] != "X": #immediate
                    opcode = "001110"
                    immediate = f"{int(components[2]):06b}"
                    encoded = opcode + immediate + first + dst
                else:
                    opcode = "001100"
                    second = f"{int(components[2][1]):02b}" #second register
                    encoded = opcode + second + "0000" + first + dst #0000 is to fill space

            #LDR
            elif instruc_type == "LDR":
                first = f"{int(components[1][1]):02b}"
                if components[2][0] != "X": #immediate
                    opcode = "101010"
                    immediate = f"{int(components[2]):06b}"
                    encoded = opcode + immediate + first + dst
                else:
                    opcode = "101000"
                    second = f"{int(components[2][1]):02b}"
                    encoded = opcode + second + "0000" + first + dst

            #STR
            elif instruc_type == "STR":
                #only has immediate version since only 2 read ports
                first = f"{int(components[1][1]):02b}"
                opcode = "010010"
                immediate = f"{int(components[2]):06b}"
                encoded = opcode + immediate + first + dst

            #MOV
            elif instruc_type == "MOV":
                if components[1][0] != "X": #immediate
                    opcode = "001011"
                    immediate = f"{int(components[1]):06b}"
                    encoded = opcode + immediate + "00" + dst #note reg1 is filler bits since using reg2 for src
                else:
                    opcode = "001001"
                    src = f"{int(components[1][1]):02b}"
                    encoded = opcode + src + "0000" + "00" + dst
                    
            #write to file encoded instruction
            if cur_address % 16 == 0:
                output.write(f"{cur_address:02x}: {int(encoded, 2):04x}") #:04x means 4 hexadecimal digits padded with 0s if needed
            else:
                output.write(f" {int(encoded, 2):04x}")
                if cur_address % 16 == 15:
                    output.write("\n")
            cur_address += 1

        #fill rest of image file
        for i in range(cur_address, 256):
            if cur_address % 16 == 0:
                output.write(f"{cur_address:02x}: 0000") #:02x means 2 hexadecimal digits padded with a 0
            else:
                output.write(f" 0000")
                if cur_address % 16 == 15:
                    output.write("\n")
            cur_address += 1
        

except Exception as e:
    print(f"Error has occured: {e}")