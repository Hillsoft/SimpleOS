bits 64

section .entry3

extern cstart

pstart:
  call cstart

.halt:
  hlt
  jmp .halt
