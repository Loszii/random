.text
.global itoascii


//leaf procedure so do not need stack (not changing callee saved)
itoascii: //converts an integer to ascii
   //X0 is temp register for shifting num
   MOV X1, 0 //num digits
   MOV X2, 1 //weight of digit to add (so can multiply then subtract from number)
   MOV X3, 10 //register with 10 for mul/div
   //X4 is used below for counter
   //X5 used to store value to be subtracted

   MOV X6, X0 //store long out of way
   ADR X7, buffer //load buffer into X7

   CBZ X0, is_zero //go to edge case label when input = 0

   //first count the digits to convert
counting:
   CBZ X0, main //counted all digits
   UDIV X0, X0, X3 //divide the long by 10 (remove one digit)
   ADD X1, X1, 1 //add one to # digits
   B counting

   //main loop
main:
   CBZ X1, exit //# of digits to convert = 0
   MOV X0, X6 //reset long after counting
   SUB X4, X1, 1 //copy number of digits to X4 so we can mutate (-1 for offset) EX: for 3rd digit (123) we divide by 10 2x
inner:
   //next we divide by 10 # digits - 1 times
   CBZ X4, add_digit //add digit once we have divided enough
   UDIV X0, X0, X3 //remove a digit
   MUL X2, X2, X3 //increase weight
   SUB X4, X4, 1 //count one less digit to iterate
   B inner //repeat process until branch to add_digit

add_digit:
   //add the digit and go back
   MUL X5, X0, X2 //multiply digit by weight EX: 5*100
   SUB X6, X6, X5 //subtract digit from our long EX: 523 we would - 500 to remove 5

   ADD X0, X0, 48 //convert to char
   STRB W0, [X7] //store in string
   ADD X7, X7, 1 //move string address
   
   MOV X2, 1 //reset weight
   SUB X1, X1, 1 //subtract # of digits
   B main //go back to main loop

is_zero:
   MOV X0, 48 //move '0' ascii code to X0
   STRB W0, [X7] //put 0 at buffer
   ADD X7, X7, 1 //move string address
   B exit //go to exit

exit:
   MOV X1, 0 //move null pointer value to register
   STRB W1, [X7] //store null pointer to end string
   ADR X0, buffer //put buffer in return
   RET //return

.data
    buffer: .fill 128, 1, 0


