.ORIG x3000

LEA R0, TEN
LDB R0, R0, #0

LEA R1, LOOP

JSRR R0

ADD R3, R3, #-1




TEN .FILL x000)

DONE HALT

LOOP ADD R0, R0, #-1
    BRNZ DONE
    BRP LOOP

.END