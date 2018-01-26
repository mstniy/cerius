[bits 16]

; Returns in ax min(ax, bx)
; (Does unsigned comparison)
wmin:
  cmp ax, bx
  jae wmin_ax_bigger
    ret
  wmin_ax_bigger:
    mov ax, bx
    ret