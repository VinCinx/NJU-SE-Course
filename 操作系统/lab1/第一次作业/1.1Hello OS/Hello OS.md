1.创建并编辑boot.asm

![image-20211023220948460](image-20211023220948460.png)

2.命令行依次输入下列指令

```
nasm boot.asm –o boot.bin
bximage  -> fd -> 1.44 -> a.img
dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
```

3.配置bochs

![image-20211023221339151](image-20211023221339151.png)

配置文件保存为bochsrc，和a.img以及boot.bin放在同一目录下

4.输入指令启动bochs

```
bochs –f bochsrc
```

跳出如下界面

![image-20211023221630859](image-20211023221630859.png)

输入c

![image-20211023222124401](image-20211023222124401.png)

成功

![image-20211023222051758](image-20211023222051758.png)

