.text
.global count_specs


//leaf procedure so do not need stack (not changing callee saved)
count_specs:
    //X0 is current address in string
    MOV X1, 0 //counter of %a
    //X2 will store current character
    MOV X3, 0 //boolean if last value is %
    MOV X4, 37 //percent sign code
    MOV X5, 97 //a ascii code

main:
    LDRB W2, [X0] //X3 stores the current character
    CBZ X2, exit //null terminator
    CMP X2, X4 //check if percent sign
    B.EQ per_found //branch to set boolean
    CMP X2, X5 //check if letter is a
    B.EQ a_found //branch to check if last character was percent
    MOV X3, 0 //reset percent boolean since it is false

end_of_loop:
    ADD X0, X0, 1 //increment address after each iteration
    B main //loop back

per_found:
    MOV X3, 1 //set percent boolean to true
    B end_of_loop //end iteration

a_found:
    CBZ X3, end_of_loop //go back if last symbol wasn't percent
    ADD X1, X1, 1 //add one to counter since we found a %a (cur is a and last was %)
    MOV X3, 0 //reset percent boolean since it is false
    B end_of_loop //go back to end

exit:
    MOV X0, X1 //put counter into return register
    RET //return

