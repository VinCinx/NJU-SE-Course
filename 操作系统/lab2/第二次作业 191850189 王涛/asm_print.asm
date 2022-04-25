section .text
global asm_print;必须要有这一句才能在C++里被调用

;整个的过程是：
;调用这个函数的时候，调用函数会将参数压栈，压栈的顺序与函数中声明的变量的顺序是相反的，例如add(a,b)函数，先压栈的是b，后压栈a
;然后通过push保存一些寄存器的值
;然后更新ebp，esp会自动根据被调用函数的参数情况等进行调整
;所以被调用函数取参数的时候，只需要在ebp的基础上进行偏移就行：偏移的时候是做加法的，在栈上向函数调用方移动以拿到参数
asm_print:
    push edx
    push ecx
    push ebx
    push eax
    push ebp
    mov ebp, esp;更新ebp的位置，让ebp指向新的栈底

    mov eax, [ebp+24];这里看似只要加20，因为上面有5个push，每个占用4个字节     但是在调用函数的时候，会push一个指针用于保存从被调用函数中返回时 应该从调用函数中开始执行的地方
    call strlen
    mov edx, eax
    mov ecx, [ebp+24]
    mov ebx, 1
    mov eax, 4
    int 80h;输出

    pop ebp
    pop eax
    pop ebx
    pop ecx
    pop edx
    ret

strlen:
    push ebx;利用ebx作为一个指针，改变ebx的值，在使用之前先进行push
    mov ebx, eax
.next:
    cmp BYTE[ebx], 0
    jz .finish
    inc ebx;是加，不是减
    jmp .next
.finish:
    sub ebx, eax
    mov eax, ebx;ebx里存放字符串的长度，将结果移动到eax里，再pop ebx   这个函数是利用eax传参的
    pop ebx
    ret