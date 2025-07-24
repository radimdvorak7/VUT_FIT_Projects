.intel_syntax noprefix
.code64


.data
    .align 16
    x_index: .long 0, 3, 6, 9, 12, 15, 18, 21
    z_index: .long 2, 5, 8, 11, 14, 17, 20, 23

    pi_scalar: .float 3.1415927
    two: .float 2.0
    one: .float 1.0
    half: .float 0.5
    zero: .float 0.0
    minus_one: .float -1.0

    K_val:  .float 0.6072529350088814

    atan_table:   # atan(2^-i) for i = 0 to 15 (in radians)
    .float 0.7853981633974483     # atan(1)
    .float 0.4636476090008061     # atan(0.5)
    .float 0.24497866312686414    # atan(0.25)
    .float 0.12435499454676144
    .float 0.06241880999595735
    .float 0.031239833430268277
    .float 0.015623728620476831
    .float 0.007812341060101111
    .float 0.0039062301319669718
    .float 0.0019531225164788188
    .float 0.0009765621895593195
    .float 0.0004882812111948983
    .float 0.00024414062014936177
    .float 0.00012207031189367021
    .float 0.00006103515617420877

    inv_pow2_table:
    .float 1.0
    .float 0.5
    .float 0.25
    .float 0.125
    .float 0.0625
    .float 0.03125
    .float 0.015625
    .float 0.0078125
    .float 0.00390625
    .float 0.001953125
    .float 0.0009765625
    .float 0.00048828125
    .float 0.000244140625
    .float 0.0001220703125
    .float 0.00006103515625

    .equ CORDIC_ITERATIONS, 15
.text

.global updateVertices_simd
updateVertices_simd:
    push rbp
    mov  rbp, rsp

    push rbx
    push rdi
    push rsi
    push r12
    push r13
    push r14
    push r15

    sub  rsp, 40
    sub rsp, 8

    mov r10, [rbp + 0x30]

    mov r11d, dword ptr [rbp + 0x38]

    vbroadcastss ymm3, dword ptr [rbp + 0x40]

    mov r12, [rbp + 0x48]

    mov r13, [rbp + 0x50]

    # rcx  -> updatedVertices_array
    # rdx  -> updatedNormals_array
    # r8   -> numVertices
    # r9   -> originalWorldX_
    # r10  -> originalWorldZ_
    # r11 -> _grid_size
    # ymm3 -> time

    xor     rsi, rsi                # x = 0
    .outer_loop:
    cmp     rsi, r11            # if x >= _grid_size, exit
    jge     .end_outer

    xor     rdi, rdi            # z = 0

    .inner_loop:
        cmp     rdi, r11
        jge     .end_inner

        # Compute linear index: index = x * _grid_size + z
        mov     rax, rsi
        imul    rax, r11            # rax = x * grid_size
        add     rax, rdi            # rax = x * grid_size + z
        
        lea rbx, [rcx + rax*4]

        vbroadcastss ymm8, minus_one[rip]
        vmovups ymm9, x_index[rip]
        vgatherdps ymm10,[rbx + ymm9*4], ymm8 # vertex.x
        vmovups ymm9, z_index[rip]
        vgatherdps ymm12,[rbx + ymm9*4], ymm8 # vertex.z

        vmovaps  ymm0, ymm10        # vertex.x
        vmovaps  ymm1, ymm12        # vertex.z
        vmovaps  ymm2, ymm3         # time
        
        mov     r14, r8
        mov     r15, r9

        mov     r8, r12
        mov     r9, r13
        call .getWaveHeight

        mov     r8, r14
        mov     r9, r15
        vmovaps  ymm11, ymm0
        #vmovups  [rbx + 4], ymm11 # vertex.y
        xor r14, r14

        sub rsp, 32                    # space for 8 floats
        vmovups [rsp], ymm11           # store ymm11 to stack
        .save_y_loop:
            
            mov r15, r14
            lea r15, [r15 + r15*2]    # r15 = i + 2*i = 3*i
            add r15, 1                # r15 = 3*i + 1
            shl r15, 2                # r15 *= 4 (float size)
            
            vmovss xmm12, dword [rsp + r14*4]
            vmovss [rbx + r15], xmm12 
            
            inc r14
            cmp r14, 8
            jl .save_y_loop
        add rsp, 32
        
        vmovups  ymm0, [r9 + rax*4]
        vmovups  ymm1, [r10 + rax*4]

        call .getWaveNormal
        vmovups [rdx + rax*4], ymm0

        add rdi, 8
        jmp     .inner_loop

        .end_inner:
    inc     rsi
    jmp     .outer_loop

    .end_outer:

    # Function epilogue
    add rsp, 8
    add rsp, 40

    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rdi
    pop rbx

    pop rbp
    ret

.getWaveHeight:
    push rbp
    mov  rbp, rsp

    push rcx
    push rbx

    # ymm0  vertex.x
    # ymm1  vertex.z
    # ymm2  time

    # CODE
    vmovaps  ymm3, ymm2
    vmovaps  ymm2, ymm1
    vmovaps  ymm1, ymm0

    vxorps ymm1, ymm1, ymm1 

    vxorps ymm0, ymm0, ymm0
    xor rax, rax
    .for_wave:
        cmp rax, r13
        jge .end
        mov rbx, rax
        imul rbx, 24           # offset = index * sizeof(GerstnerWave)
        add rbx, r12           # address of wave[n]
        
        mov rcx, rbx
        call .getGerstnerWaveHeight # return value in ymm4
        vaddps ymm0, ymm0, ymm4
        inc rax
        jmp .for_wave
    .end:

    pop rbx
    pop rcx

    pop rbp
    ret

.getGerstnerWaveHeight:
    push rbp
    mov  rbp, rsp

    # ecx      GerstnerWave*
    # ymm1    vertex.x
    # ymm2     vertex.z
    # ymm3     time

    # CODE
    vbroadcastss ymm4, two[rip] # 2.0
    vbroadcastss ymm5, pi_scalar[rip]
    vmulps ymm4, ymm4, ymm5 # 2.0 * pi
    vmovaps ymm6, ymm4

    vmulps ymm4, ymm4, ymm3 # 2.0 * pi * time

    vbroadcastss ymm5, [rcx + 4] # wavelength

    vdivps ymm4, ymm4, ymm5 # (2.0 * pi * time)/wavelength
    vdivps ymm6, ymm6, ymm5 # k = (2.0 * pi) / wavelenght


    # Call sinus on ymm4
    # Backup the arguments
    vmovaps ymm12, ymm0
    vmovaps ymm13, ymm1
    vmovaps ymm14, ymm2
    vmovaps ymm15, ymm3

    vmovaps ymm0, ymm4

    call .cordic_sin_cos

    vmovaps ymm4, ymm10
    # Restore arguments
    vmovaps ymm0, ymm12
    vmovaps ymm1, ymm13
    vmovaps ymm2, ymm14
    vmovaps ymm3, ymm15

    vbroadcastss ymm12, [rcx + 4] # wavelenght
    vbroadcastss ymm13, [rcx + 8] # speed
    vmulps ymm5, ymm6, ymm13    # w = ((2.0 * pi) / wavelenght) * speed

    vbroadcastss ymm11, one[rip] # 1
    vbroadcastss ymm12, half[rip] # 0.5
    vbroadcastss ymm13, [rcx] # amplitude

    vaddps ymm4, ymm4, ymm11 # sin + 1
    vmulps ymm4, ymm4, ymm12 # (sin + 1) * 0.5
    vmulps ymm4, ymm4, ymm13 # periodicAmplitude = (sin + 1) * 0.5 * amplitude

    ## Dot product
    vbroadcastss ymm12, [rcx + 12]   # wave.x 
    vbroadcastss ymm13, [rcx + 16]   #wave.z

    vmulps ymm12, ymm12, ymm1 # wave.x * vertex.x
    vmulps ymm13, ymm12, ymm2 # wave.z * vertex.z
    vaddps ymm12, ymm12, ymm13 # dotProduct

    vmulps ymm6, ymm6, ymm12 # k * dotProduct
    vmulps ymm5, ymm5, ymm3 # w * time
    vminps ymm5, ymm6, ymm5 # k * dotProduct - w * time

    vbroadcastss ymm12, [rcx + 20] # phase
    vaddps ymm5, ymm5, ymm14

    # Call sinus on ymm5
    # Backup the arguments
    vmovaps ymm12, ymm0
    vmovaps ymm13, ymm1
    vmovaps ymm14, ymm2
    vmovaps ymm15, ymm3

    vmovaps ymm0, ymm5

    call .cordic_sin_cos
    
    vmovaps ymm5, ymm1
    # Restore arguments
    vmovaps ymm0, ymm12
    vmovaps ymm1, ymm13
    vmovaps ymm2, ymm14
    vmovaps ymm3, ymm15

    vmulps ymm4, ymm4, ymm5

    pop rbp
    ret

.getWaveNormal:
    push rbp
    mov  rbp, rsp

    push rcx
    push rbx
    push rax
    # ecx      GerstnerWave*
    # ymm0    Originalx
    # ymm1     Originalz
    # ymm2     time

    xor rax, rax
    .for_wave_normal:
        cmp rax, r13
        jge .end_normal
        mov rbx, rax
        imul rbx, 24           # offset = index * sizeof(GerstnerWave)
        add rbx, r12           # address of wave[n]

        vbroadcastss ymm10, two[rip] # 2.0
        vbroadcastss ymm11, pi_scalar[rip]

        vmulps ymm10, ymm10, ymm11 # 2.0 * pi

        vbroadcastss ymm12, [rcx + 4] # wavelenght
        vbroadcastss ymm13, [rcx + 8] # speed

        vdivps ymm10, ymm10, ymm12   # k = (2.0 * pi) / wavelenght
        vmulps ymm14, ymm10, ymm13   # w = ((2.0 * pi) / wavelenght) * speed

        vbroadcastss ymm5, [rcx + 12]   # wave.x 
        vbroadcastss ymm6, [rcx + 16]   #wave.z

        vmulps ymm5, ymm5, ymm0 # wave.x * OriginalX
        vmulps ymm6, ymm6, ymm1 # wave.z * OriginalZ
        vaddps ymm6, ymm6, ymm5 # dotProduct

        vmulps ymm11, ymm10, ymm6 # k * dotProduct
        vmulps ymm14, ymm14, ymm2 # w * time
        vminps ymm14, ymm11, ymm14 # k * dotProduct - w * time

        vmovaps ymm12, ymm0
        vmovaps ymm0, ymm14
        vmovaps ymm13, ymm1
        vmovaps ymm14, ymm2

        call .cordic_sin_cos

        vmovaps ymm8, ymm0
        vmovaps ymm9, ymm1

        # Restore arguments
        vmovaps ymm0, ymm12
        vmovaps ymm1, ymm13
        vmovaps ymm2, ymm14

        vmovaps ymm15, ymm9 # cos_term
        vmovaps ymm3, ymm8 # sin_term

        vbroadcastss ymm10, two[rip] # 2.0
        vbroadcastss ymm11, pi_scalar[rip]

        vmulps ymm10, ymm10, ymm11 # 2.0 * pi

        vbroadcastss ymm12, [rcx + 8] # speed
        vmulps ymm10, ymm10, ymm2  #2.0 * pi *time
        vmulps ymm10, ymm10, ymm12    # (2.0 * pi *time) / wavelenght

        vmovaps ymm14, ymm0
        vmovaps ymm13, ymm1
        vmovaps ymm12, ymm2

        call .cordic_sin_cos

        vmovaps ymm9, ymm1

        # Restore arguments
        vmovaps ymm2, ymm12
        vmovaps ymm1, ymm13
        vmovaps ymm0, ymm14

        vbroadcastss ymm11, one[rip] # 1
        vbroadcastss ymm12, half[rip] # 0.5
        vbroadcastss ymm13, [rcx] # amplitude

        vaddps ymm9, ymm9, ymm11 # sin + 1
        vmulps ymm9, ymm9, ymm12 # (sin + 1) * 0.5
        vmulps ymm9, ymm9, ymm13 # modulatedAmplitude = (sin + 1) * 0.5 * amplitude

        vmovaps ymm14, ymm8
        
        vbroadcastss ymm10, two[rip] # 2.0
        vbroadcastss ymm11, pi_scalar[rip]

        vmulps ymm10, ymm10, ymm11 # 2.0 * pi

        vbroadcastss ymm12, [rcx + 4] # wavelenght
        vbroadcastss ymm13, [rcx + 8] # speed

        vdivps ymm10, ymm10, ymm12   # k = (2.0 * pi) / wavelenght

        # ymm15 cos_term
        # ymm14 sin_term
        # ymm10 k
        # ymm9 modulatedAmplitude

        vbroadcastss ymm3, one[rip] # TangentX.1
        vbroadcastss ymm4, zero[rip] # TangentX.2
        vbroadcastss ymm5, zero[rip] # TangentX.3

        vmulps ymm0, ymm14, ymm10
        vmulps ymm0, ymm0, ymm9 # k * modAmp * sin_term

        vmulps ymm1, ymm15, ymm10
        vmulps ymm1, ymm1, ymm9 # k * modAmp * cos_term
        
        vbroadcastss ymm12, [rcx + 12]   # wave.x 
        vbroadcastss ymm13, [rcx + 16]   # wave.y

        vmulps ymm14, ymm12, ymm1
        vmulps ymm15, ymm13, ymm0

        vbroadcastss ymm6, zero[rip] # TangentZ.1
        vbroadcastss ymm7, zero[rip] # TangentZ.2
        vbroadcastss ymm8, one[rip] # TangentZ.3
        
        inc rax
        jmp .for_wave_normal
    .end_normal:

    pop rax
    pop rbx
    pop rcx
    pop rbp
    
    ret

.cordic_sin_cos:
    push rbp
    mov  rbp, rsp

    push rcx
    push rbx
    push rax
    # Free registers: 0,1,2,3, 7-11

    vmovaps   ymm2, ymm0        # ymm2 = z = angle
    vbroadcastss   ymm0, one[rip]    # ymm0 = x = 1.0
    vxorps   ymm1, ymm1, ymm1        # ymm1 = y = 0.0
   
    xor     rcx, rcx          # rcx = i = 0

    .loop:
        vmovaps   ymm3, ymm2        # copy z
        vxorps   ymm7, ymm7, ymm7    # ymm7 = 0
        vcmpps  ymm8, ymm3, ymm7, 5   #mask
        
        # x_new = x - y * 2^-i
        lea rax, inv_pow2_table[rip]
        vbroadcastss ymm9, [rax + rcx*4]
        vmovaps   ymm10, ymm1        # ymm6 = y
        vmulps   ymm10, ymm10, ymm9        # ymm6 = y >> i
        vandps ymm10, ymm10, ymm8
        vsubps   ymm0, ymm0, ymm10        # x -= y >> i

        # y_new = y + x >> i
        vmovaps   ymm10, ymm0
        vmulps   ymm10, ymm10, ymm9
        vandps ymm10, ymm10, ymm8
        vaddps   ymm1, ymm1, ymm10        # y += x >> i

        # z = z - atan(2^-i)
        lea rax, atan_table[rip]
        vbroadcastss ymm11, [rax + rcx*4]
        vandps ymm11, ymm11, ymm8
        vsubps   ymm2, ymm2, ymm11

        vxorps ymm9, ymm8, ymm8
        vxorps ymm9, ymm9, ymm9 # all 1

        vxorps ymm8, ymm8, ymm9

        # negative
        # x_new = x + y >> i
        lea rax, inv_pow2_table[rip]
        vbroadcastss ymm9, [rax + rcx*4]
        
        vmovaps   ymm10, ymm1
        vmulps   ymm10,  ymm10, ymm9
        vandps ymm10, ymm10, ymm8
        vaddps   ymm0, ymm0, ymm10

        # y_new = y - x >> i
        vmovaps   ymm10, ymm0
        vmulps   ymm10, ymm10, ymm9
        vandps ymm10, ymm10, ymm8
        vsubps   ymm1, ymm1, ymm10

        # z = z + atan(2^-i)
        lea rax, atan_table[rip]
        vbroadcastss ymm11, [rax + rcx*4]
        vandps ymm11, ymm11, ymm8
        vaddps   ymm2, ymm2, ymm11

    .next:
    inc     rcx
    cmp     rcx, CORDIC_ITERATIONS
    jl      .loop

    vbroadcastss ymm11, K_val[rip]

    # cos in ymm0
    vmulps ymm0, ymm0, ymm11
    # sin in ymm1
    vmulps ymm1, ymm1, ymm11

    pop rax
    pop rbx
    pop rcx
    pop rbp
    ret

