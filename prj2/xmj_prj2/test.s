            addi $a1, $zero, 10
            add  $a0, $zero, $zero
            add  $a2, $zero, $zero
loop:       addi $a0, $a0, 1
            beq  $a0, $a1, nextloop            ! add a0 to 10 and go to next loop
            beq  $a0, $a0, loop
nextloop:
            sw $a2, 0($zero)                   ! increase the value at 0x0 from 0 to 10
            addi $a2, $a2, 1
            beq $a2, $a1, end                 
            beq $a2, $a2, nextloop
end:        sw $a2, 0($zero)
            lw $a2, 1($zero)                   ! just want to test if lw works or not

halt:       beq $zero, $zero, halt             ! go on a infinite loop and that's the end


