!============================================================
! CS-2200 Homework 1
!
! Please do not change mains functionality, 
! except to change the argument for fibonacci or to meet your 
! calling convention
!============================================================

main:       la $sp, stack				! load address of stack label into $sp
            noop                        ! FIXME: load desired value of the stack 
                                        ! (defined at the label below) into $sp
            la $at, fibonacci	        ! load address of fibonacci label into $at
            addi $a0, $zero, 8 	        ! $a0 = 8, the fibonacci argument
            jalr $at, $ra				! jump to fibonacci, set $ra to return addr
            halt						! when we return, just halt


fibonacci:  noop                        ! FIXME: change me to your fibonacci implementation

stack:	    .word 0x4000				! the stack begins here

