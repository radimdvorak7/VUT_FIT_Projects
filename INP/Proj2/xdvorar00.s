; Autor reseni: Radim Dvorak xdvorar00

; Projekt 2 - INP 2024
; Vigenerova sifra na architekture MIPS64

; DATA SEGMENT
                .data
msg:            .asciiz "radimdvorak"
cipher:         .space  31 ; misto pro zapis zasifrovaneho textu

; zde si muzete nadefinovat vlastni promenne ci konstanty,
key:            .byte 4, 22, 15 ; values for 'd', 'v' and 'o'

params_sys5:    .space  8 ; misto pro ulozeni adresy pocatku
                          ; retezce pro vypis pomoci syscall 5
                          ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

main:
                ; r8 - key index
                ; r9 - key
                ; r10 - add/sub
                ; r11 - working (change) char
                ; r12 - message index
                ; r13 - char overflow/underflow
                ; r20 - permanent 3, max value of key index
                ; r23 - number of chars for correcting underflow and overflow

                addiu r20, r0, 3
                addiu r23, r0, 26
next_char:      
                lbu r9, key(r8)
                lbu r11, msg(r12)
                beqz r11, end               ; loaded char has value of zero
                beqz r10, add_value         ; for char at even index add value of key
                addi r16, r0, 97
                subu r11, r11, r9  
                slt r13, r11, r16           ; r13 = 1, when r11 < 'a'
                beqz r13, char_underflow    ; if r == 0, skip underflow correction
                addu r11, r11, r23
char_underflow:
                b write
add_value:         
                addi r16, r0, 122
                addu r11, r11, r9
                slt r13, r16, r11           ; r13 = 1, when r11 > 'z'
                beqz r13, write             ; if r == 0, skip overflow correction
                subu r11, r11, r23
write:
                sb r11, cipher(r12)
                addiu r12 ,r12, 1
                addiu r8, r8, 1
                sltiu r21, r12, 30
                xori r10, r10 , 1           ; change mode of modifying of char (incrementing or decrementig)
                sltu r18, r8, r20
                beqz r21, end
                bnez r18, reset_key_index   
                movz r8, r0 ,r0
reset_key_index:
                b next_char
end:            
                sb r0, cipher(r12)
                daddi   r4, r0, cipher
                jal     print_string


; NASLEDUJICI KOD NEMODIFIKUJTE!

                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
