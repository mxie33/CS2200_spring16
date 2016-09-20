!============================================================
! CS-2200 Homework 1
!
! Please do not change mains functionality, 
! except to change the argument for fibonacci or to meet your 
! calling convention
!============================================================

main:       la $sp, stack               ! load address of stack label into $sp
            lw $sp, 0($sp)              ! FIXME: load desired value of the stack 
                                        ! (defined at the label below) into $sp
            la $at, fibonacci           ! load address of fibonacci label into $at
            addi $a0, $zero, 8          ! $a0 = 8, the fibonacci argument
            jalr $at, $ra               ! jump to fibonacci, set $ra to return addr
            halt                        ! when we return, just halt


fibonacci:
            addi $sp, $sp, -5           ! Yield 5 spaces
            sw $fp, 1($sp)              ! Store the frame pointer
            sw $ra, 2($sp)              ! Store the return address
            sw $a0, 4($sp)              ! Store the argument into the stack
            addi $fp, $sp, 0            ! $fp = $sp - 0
            addi $t1, $zero, 1          ! $t1 = 1

            beq $a0, $zero, zero        ! if a0 = 0
            beq $a0, $t1, one           ! if a0 = 1

            addi $a0, $a0, -1           ! $a0 = n - 1
            jalr $at, $ra               ! fib(n-1)

            sw $v0, 0($sp)              ! Store the value of fib(n-1)
            lw $a0, 4($fp)              ! Load the argument
            addi $a0, $a0, -2           ! a0 = n - 2
            jalr $at, $ra               ! fib(n-2)

            lw $s0, 0($sp)              ! load the result of fib(n-1)
            add $v0, $v0, $s0           ! fib(n-1) + fib (n-2)
            beq $zero, $zero, RETURN    ! return
zero:
            add $v0, $zero, $zero       ! $v0 = 0
            beq $zero, $zero, RETURN    ! return 0
one:
            addi $v0, $zero, 1          ! $v0 = 1

RETURN:
            sw $v0, 3($fp)              ! Store the return value
            addi $sp, $fp, 3            ! load the stack pointer point to the return value
            lw $ra, 2($fp)              ! load the return address
            lw $fp, 1($fp)              ! load the old frame pointer
            addi $sp, $sp, 2            ! Pop out the argument
            jalr $ra, $zero             ! jump to the return address

stack:      .word 0x4000                ! the stack begins here

