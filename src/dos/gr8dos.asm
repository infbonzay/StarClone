.386
_DATA   SEGMENT DWORD PUBLIC 'DATA'


VIDEOADR         equ dword ptr 0a0000h

        _DPMIReg label dword
        _DPMIReg_EDI      DD 0
        _DPMIReg_ESI      DD 0
        _DPMIReg_EBP      DD 0
        _DPMIReg_RESV     DD 0
        _DPMIReg_EBX      DD 0
        _DPMIReg_EDX      DD 0
        _DPMIReg_ECX      DD 0
        _DPMIReg_EAX      DD 0
        _DPMIReg_FLAGS    DW 0
        _DPMIReg_ES       DW 0
        _DPMIReg_DS       DW 0
        _DPMIReg_FS       DW 0
        _DPMIReg_GS       DW 0
        _DPMIReg_IP       DW 0
        _DPMIReg_CS       DW 0
        _DPMIReg_SP       DW 0
        _DPMIReg_SS       DW 0
        linenr             dd 0
        _wlinecount        dd 1024+256        dup(0)
        _GRP_wmaxdworddesen    dd 1024+256        dup(0)
        _wendlineaddfactor dd 1024+256        dup(0)
        _GRP_wmaxbank          dd 0
        _GRP_wmaxbankl         dd 0
        _wgranularity      dd 0

_DATA   ENDS

_TEXT32 segment dword public 'CODE' use32


assume cs:_TEXT32,ds:_DATA

public  _DPMIReg_IP,_DPMIReg_CS
public  _DPMIReg_EBX,_DPMIReg_EDX
public wvertint_,wscreenon_,wreadpallette_,wwritepalette_
public _wlinecount,_GRP_wmaxdworddesen,_wendlineaddfactor,_GRP_wmaxbank,_GRP_wmaxbankl
public _wgranularity

;=================================
wvertint_ proc near
        push edx
        push eax
        mov edx,3dah
no_vertline:
        in al,dx
        test al,8
        jz no_vertline

yes_vertline:
        in al,dx
        test al,8
        jnz yes_vertline

;no_vertline1:
;        in al,dx
;        test al,8
;        jz no_vertline1
        pop eax
        pop edx
        ret
wvertint_ endp
;===================
wscreenon_ proc near; char *vscreen
;    cli
    push    es
    push    ds
    push    ds
    pop     es
    push    eax
    push    ebx
    push    ecx
    push    edi
    push    esi
    xor     edx,edx
    mov     esi,eax
    mov     linenr,edx
    mov     _DPMIReg_EBX,0
nextbank:
    mov     _DPMIReg_EDX,edx
    mov     eax,0301h
    xor     ebx,ebx
    xor     ecx,ecx
    mov     edi,offset _DPMIReg
    int     31h
    mov     edi,VIDEOADR
    mov     ebx,[edx*4+_wlinecount]
    mov     eax,linenr
nextline:
    mov     ecx,[eax*4+_GRP_wmaxdworddesen]
nowaitvertline:
    rep     movsd
    add     esi,[eax*4+_wendlineaddfactor]
    inc     eax
    dec     ebx
    jnz     nextline
    mov     linenr,eax
    inc     edx
    cmp     edx,dword ptr _GRP_wmaxbank
    jb      nextbank
    pop     esi
    pop     edi
    pop     ecx
    pop     ebx
    pop     eax
    pop     ds
    pop     es
;    sti
    ret
wscreenon_ endp
;===========================================
wreadpalette_ proc near ;EAX-adrofmem
push edi
push edx
push ecx
mov edi,eax
mov edx,03c7h
mov al,0
out dx,al
add edx,2
mov ecx,256
_r1:
push ecx
mov ecx,3
_r2:
in al,dx
mov [edi],al
inc edi
dec ecx
jnz _r2
pop ecx
dec ecx
jnz _r1
pop ecx
pop edx
pop edi
ret
wreadpalette_ endp
;=============================
wwritepalette_ proc near       ;EAX-adrofmem
push esi
push edx
push ecx
mov esi,eax
mov edx,03c8h
mov al,0
out dx,al
inc edx
mov ecx,256
_w1:
push ecx
mov ecx,3
_w2:
mov al,[esi]
inc esi
out dx,al
dec ecx
jnz _w2
pop ecx
dec ecx
jnz _w1
pop ecx
pop edx
pop esi
ret
wwritepalette_ endp
;=============================
_TEXT32 ends
end


