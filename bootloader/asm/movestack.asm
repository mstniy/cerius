[BITS 64]
global MoveStack

MoveStack:
  sub rbp, rsp
  add rbp, rdi
  mov rcx, rsi
  mov rsi, rsp
  mov rdi, rdi
  mov rsp, rdi
  rep movsb
  ret