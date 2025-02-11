.text
.global concat_array


concat_array:
   SUB SP, SP, 64 //allocate room for storing
   STR X30, [SP] //store link register

   //X0 will have long in it below
   //X1 is address of .data string
   MOV X2, X0 //put address of array into X2
   MOV X3, X1 //put length into X3
   
   ADR X1, concat_array_outstr //address of .data string (must be after we move X1 to X3)

   MOV X4, 0 //number of longs converted
   MOV X5, 0 //offset in .data string
   MOV X6, 8 //8 register for multiplication
   //X7 is temp used for storage of things

main:
   CMP X4, X3 // compare # converted to length
   B.EQ exit //exit if have converted all of them

   //getting the long in string format
   MUL X7, X4, X6 //# converted * 8 for offset
   LDR X0, [X2, X7] //put long into X0

   //below will change X1-X7 so we save those to stack (dont need to save X0)
   STR X1, [SP, 8]
   STR X2, [SP, 16]
   STR X3, [SP, 24]
   STR X4, [SP, 32]
   STR X5, [SP, 40]
   STR X6, [SP, 48]
   STR X7, [SP, 56]

   BL itoascii

   //load back, now X0 is string version of old long
   LDR X1, [SP, 8]
   LDR X2, [SP, 16]
   LDR X3, [SP, 24]
   LDR X4, [SP, 32]
   LDR X5, [SP, 40]
   LDR X6, [SP, 48]
   LDR X7, [SP, 56]

copy_str:
   LDRB W7, [X0] //load character from long string
   CBZ X7, copy_exit //null terminator
   STRB W7, [X1, X5] //store character from long string
   ADD X0, X0, 1 //move long string address forward
   ADD X5, X5, 1 //move .data offset forward
   B copy_str //loop back

copy_exit:
   ADD X4, X4, 1 //add one to # long converted
   MOV X7, 32 //ascii code for space
   STRB W7, [X1, X5] //add space at end
   ADD X5, X5, 1 //move .data offset forward
   B main //go back to main

exit:
   STRB WZR, [X1, X5] //add null terminator
   LDR X30, [SP] //restore link register
   ADD SP, SP, 64 //unallocate stack
   ADR X0, concat_array_outstr //return value of address
   RET //return

.data
    concat_array_outstr:  .fill 1024, 1, 0

