This file contains the assembler version of the procedure
'detect_IRQ'. Mike Surikov wrote the C source, and I've translated
it to assembly language. Simply adjust BASEADDR, assemble it
(with MASM or TASM) and run the .EXE file you get.

Chris

----8<--------8<--------8<--------8<--------8<--------8<--------8<----

model small

BASEADDR  equ 03f8h

.data?
mask  db  ?
irq   db  ?
imr   db  ?
ier   db  ?
lcr   db  ?
mcr   db  ?

.data
nonetext  db "NONE$"
irqtext   db "IRQ level $"

.code

detect_IRQ  proc near
  ; Call this routine with the base address of the UART
  ; in DX. It returns the interrupt level in AX or -1
  ; if none.
  ; No registers except AX and flags are changed.

  push  bx
  push  dx

  cli               ; disable interrupts

  add  dx,3         ; DX points to the LCR
  in   al,dx        ; read LCR
  
  mov  lcr,al       ; and store it for later use
  and  al,7fh       ; clear DLAB
  out  dx,al        ; and rewrite value of LCR

  sub  dx,2         ; DX points to the IER
  in   al,dx        ; read IER
  mov  ier,al       ; and store it for later use

  xor  al,al        ; disable all UART interrupts
  out  dx,al        ; separately

  add  dx,3         ; DX now points to the MCR
  in   al,dx        ; read the MCR
  mov  mcr,al       ; and store its value

  and  al,NOT 10h   ; clear the loopback bit
  or   al,0ch       ; and allow UART interrupts
  out  dx,al        ; and write this value back

  in   al,21h       ; get the interrupt mask register
  mov  imr,al       ; and store its value

  mov  al,0ah       ; prepare to read the IRR of
  out  20h,al       ; the ICU

  ; at this point the transmitter must already be empty

  mov  bl,0fch      ; the mask for IRQ 2-7

  sub  dx,3         ; DX now points to the IER
  mov  al,2         ; allow 'Transmitter Empty'
  out  dx,al        ; interrupts

  in   al,20h       ; check if an interrupt request
  and  bl,al        ; appeared

  xor  al,al        ; disable 'Transmitter Empty'
  out  dx,al        ; interrupt again

  in   al,20h       ; check if the interrupt request
  not  al           ; disappeared again
  and  bl,al

  mov  al,2         ; enable 'Transmitter Empty'
  out  dx,al        ; interrupt again

  in   al,20h       ; and check again if it appeared
  and  al,bl        ; in the ICU

  not  al
  out  21h,al       ; unmask the interrupt found yet

  mov  al,0ch       ; enter the poll mode of the ICU
  out  20h,al

  in   al,20h       ; accept the interrupt (we get
  mov  bl,al        ; the int level in bit 0-2)
                    ; now clear all possible int sources
  add  dx,4         ; DX now points to the LSR
  in   al,dx
  sub  dx,5         ; DX now points to the RBR
  in   al,dx
  add  dx,2         ; DX now points to the IIR
  in   al,dx
  add  dx,4         ; DX now points to the MSR
  in   al,dx

  mov  al,ier       ; restore old value of the IER
  sub  dx,5         ; DX now points to the IER
  out  dx,al
  mov  al,lcr       ; restore old value of the LCR
  add  dx,2         ; DX now points to the LCR
  out  dx,al
  mov  al,mcr       ; restore old value of the MCR
  inc  dx           ; DX now points to the MCR
  out  dx,al

  mov  al,20h       ; signal end of poll mode
  out  20h,al       ; to the ICU

  mov  al,imr       ; restore old interrupt mask
  out  21h,al

  sti               ; and enable all interrupts again

  mov  ax,-1        ; default: 'no int'
  test bl,80h       ; see if any interrupt appeared
  jz   det_int_1
  mov  ax,bx
  and  ax,7         ; strip all bits we don't need
det_int_1:
  pop  dx
  pop  bx
  ret
detect_IRQ  endp

  ; here we test it.

start:
  mov  ax,DGROUP
  mov  ds,ax
  mov  dx,BASEADDR
  call detect_IRQ
  cmp  ax,-1
  jne  not_none
  lea  dx,nonetext
  mov  ah,9
  int  21h
  jmp  end
not_none:
  push ax
  lea  dx,irqtext
  mov  ah,9
  int  21h
  pop  dx
  add  dl,'0'
  mov  ah,2
  int  21h
end:
  mov  ax,4c00h
  int  21h

.stack 256

end start

---->8-------->8-------->8-------->8-------->8-------->8-------->8----



