        // ARM assembly program to compute factorial
        // Put the required header information here

        // References
        // https://godbolt.org/
        // https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Learn%20the%20Architecture/Armv8-A%20Instruction%20Set%20Architecture.pdf?revision=818c7869-3849-4e5d-bde2-710e69defb57
        // https://developer.arm.com/documentation/den0024/a/ARMv8-A-Architecture-and-Processors/ARMv8-A-Processor-properties/ARMv8-processors
        // https://cupola.gettysburg.edu/oer/8/
        // https://github.com/d-gaston/armv8-examples

        .extern printf
        .extern scanf
        .global main
        .func main
main:
        stp     x29, x30, [sp, #-16]!
        ldr     x0, =prompt     // Get address of prompt
        bl      printf          // Print prompt to screen
        ldr     x0, =in         // Get addr of input format string
        ldr     x1, =n          // Get addr of n
        bl      scanf           // Put value in n using scanf
        ldr     x0, =n          // Addr of n into x0
        ldr     x3, [x0]        // n is now in x3
        //-------------
        // Your code goes here.  Put n! in x2
        //-------------
        // Start by putting 1 into x2

        // Give value explicitly, for some reason reading
        // does not work with the loop
        mov  x3, 3
lp:     // create a label for the beginning of your loop
        // Write a loop which multiples x3 by x2 each iteration
        // Decrementing x3 until x3 reaches 1
        // (Alternatively one could write a loop with an index register
        //  that counts up from 1 to n.)


        // Compare and branch not zero, jump
        // to line lp if register x3 is not zero


        //-------------
        // Do NOT alter code past this line.
        //-------------
finish: mov     x1, x2
        ldr     x0, =out
        bl      printf
        ldp     x29, x30, [sp], #16
        // Exit
        mov x8, #93
        svc 0

        .data
n:              .word 0
prompt:         .asciz "Enter n: "
in:             .asciz "%d"
out:            .asciz "factorial is: %d\n"
