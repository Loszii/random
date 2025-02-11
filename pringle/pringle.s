.text
.global pringle


pringle:
    SUB SP, SP, 136 //allocate stack (room for X30, 7 params, and X0-X8 = 1+7+9 = 17*8 = 136)
    STR X30, [SP] //store link reg on stack
    //put X1-X7 params onto stack (leave space for registers later before BL)
    STR X1, [SP, 80]
    STR X2, [SP, 88]
    STR X3, [SP, 96]
    STR X4, [SP, 104]
    STR X5, [SP, 112]
    STR X6, [SP, 120]
    STR X7, [SP, 128]

    //register data:
    //X0 is the address of format string
    ADR X1, pringle_str //put address of resultant string in X1
    MOV X2, 0 //counter for characters to print (and offset)
    //X3 will store current character
    MOV X4, 0 //boolean if last value is %
    MOV X5, 37 //percent sign code
    MOV X6, 97 //a ascii code
    MOV X7, 80 //X7 stores stack offset of current array param
    MOV X8, 88 //X8 stores stack offset of current array length param
    //X9 will be temp register to place return val

    //iterate through format string, until we get a %a
main:
    LDRB W3, [X0] //get character from format string
    CBZ X3, exit //exit if null term (done)
    CMP X3, X5 //compare current character if %
    B.EQ per_found //found percent
    CMP X3, X6 //compare if current is a
    B.EQ a_found //found a
    MOV X4, 0 //reset boolean for specifier

bottom_main:
    //copy current character to result since just regular character
    STRB W3, [X1, X2]
    ADD X0, X0, 1 //move format string forward
    ADD X2, X2, 1 //increase num chars to print (and offset)
    B main

per_found:
    MOV X4, 1 //set percent bool to 1
    B bottom_main //branch back

a_found:
    CBZ X4, bottom_main //no percent since flag is 0 so go back (regular a)
    MOV X4, 0 //reset percent flag
    //must remove the percent that we stored in string previously since it is a specifier
    SUB X2, X2, 1 //go back to percent sign, will override it

    //have found %a so we must concat the array and then place in final string:
    //first we save our registers with data in them
    STR X0, [SP, 8]
    STR X1, [SP, 16]
    STR X2, [SP, 24]
    STR X3, [SP, 32]
    STR X4, [SP, 40]
    STR X5, [SP, 48]
    STR X6, [SP, 56]
    STR X7, [SP, 64]
    STR X8, [SP, 72]

    //put params into register
    LDR X0, [SP, X7] //address of array
    LDR X1, [SP, X8] //length of array
    BL concat_array //call concat array
    MOV X9, X0 //place return value (string address) in X9

    //load back registers
    LDR X0, [SP, 8]
    LDR X1, [SP, 16]
    LDR X2, [SP, 24]
    LDR X3, [SP, 32]
    LDR X4, [SP, 40]
    LDR X5, [SP, 48]
    LDR X6, [SP, 56]
    LDR X7, [SP, 64]
    LDR X8, [SP, 72]

    //copy concat string to our final string
copy_loop:
    LDRB W3, [X9] //get character from concat array string
    CBZ X3, exit_copy //null terminator so exit
    STRB W3, [X1, X2] //store character in final string
    ADD X2, X2, 1 //move offset
    ADD X9, X9, 1 //move concat string
    B copy_loop

exit_copy:
    //move to next parameter
    ADD X0, X0, 1 //move format string forward
    ADD X7, X7, 16 //add address parameter offset
    ADD X8, X8, 16 //add length parameter offset
    B main

exit: //to-do de-alloc stack print string return chars printed
    //print string
    MOV X0, 1
    //X1 still is address of final string
    //X2 still is length
    MOV X8, 64 //print service code
    SVC 0 //actually print the string

    MOV X0, X2 //put number characters printed in return address
    LDR X30, [SP] //load back link reg
    ADD SP, SP, 136 //add back stack
    RET //return

.data
    pringle_str: .fill 1024, 1, 0 //string to store result
    