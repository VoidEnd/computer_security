

# Buffer_Overflow

## 知识点

1. **[question]**返回地址和前栈指针，帧指针寄存器

2. **地址空间随机化**（address space layout randomization， ASLR），用`sudo sysctl -w kernel.randomize_va_space=0`关闭。可攻破。

3. **argc** ：是 argument count的缩写，表示argv这个二级指针指向的内存区域中保存的由stub写入的有效命令行参数的个数.

   **argv** ：是 argument vector的缩写，表示传入main函数的参数序列或指针，并且第一个参数argv[0]一定是程序的名称，并且包含了程序所在的完整路径，所以确切的说需要输入的main函数的参数个数是argc-1个。

4. **不可执行栈**（non-executable stack）：默认情况下，一个程序的栈是不可执行的，因此在栈中注入的恶意代码也是无法执行的。可由return-to-libc攻破。可用`-z execstack`关闭。

5. **StackGuard**：能够抵御基于栈的缓冲区溢出攻击，主要思想是在代码中添加一些特殊数据和检测机制，从而可以检测到缓冲区溢出的发生，已被gcc采纳。可用`-fno-stack-protector`关闭。

6. 在引入一些防御措施之前，大多数系统会把栈（每个进程有一个栈）放在固定的起始位置。

7. **nop**：这个指令在汇编中的作用是空指令，意味着什么都不做的意思，一般用来控制CPU的时间周期，达到时钟延时的效果。可以通过填充nop指令，为恶意代码创建大量入口点。P70

8. **touch**：用于修改文件或者目录的时间属性，包括存取时间和更改时间。若文件不存在，系统会建立一个新的文件。

9. **$**的常见用法：

   - 显示脚本参数（`$0、$?、$\*、$@、$#、$$、$!`）（本质上属于变量替换）
   - 获取变量与环境变量的值
   - 组合用法，如shell中$(( ))、$( )、${ }，功能各不相同。

10. **[question]**P72

11. **#!**：是特殊的表示符，其后面跟的是解释此脚本的shell路径。

12. **gdb调试时**会向栈里压入一些额外的数据，这将导致调试时的栈帧可能比直接运行程序时的栈帧更深一些。**恶意输入文件**中至入口之前不应出现“\0”，会导致复制提前停止。

13. **shellcode**：因为加载器问题和代码中的0,，导致不能使用由C语言程序生成的二进制代码作为恶意代码，需要直接用**汇编语言**来编写。

14. **汇编语言**：

15. **防御措施概述：**

    - 更安全的函数
    - 更安全的动态链接库，libsafe函数库，libmib函数库
    - 程序静态分析，目的是在开发早期警告开发者程序中的潜在不安全代码
    - 编程语言，由编程语言本身进行缓冲区溢出的检测，如Java、Python
    - 编译器：可以控制栈的布局，同时也可以在二进制程序中插入验证栈完整性的指令。Stackshield的方法是将返回地址备份到一个安全的地方，在函数返回之前进行返回地址对比；StackGuard的方法是在返回地址与缓冲区之间设置一个哨兵（canary），当返回地址被修改时哨兵值同样也会被修改，而哨兵值是一个随机数，他的原始备份存放在一个栈以外不会受到缓冲区溢出影响的地方，在函数返回之前进行哨兵值对比。
    - 操作系统，需要把程序加载进来并设置好运行环境，在此阶段操作系统可以指定进程的内存布局。地址空间随机化（address space layout randomization， ASLR），增加攻击难度。
    - 硬件体系结构，现代CPU支持一种叫NX bit的特性，即No-Execute，“不可执行”，是一种将代码和数据分离的技术，可以将某些内存区域设置为不可执行，可以通过这个特性将栈设置为不可执行。

16. **ELF**(Executable and Linkable Format)即可执行连接文件格式，

17. **熵**：衡量地址空间随机程度。在32位linux系统中，栈的熵为19bit，堆的熵为13bit，很容易被暴力破解，而64位系统则会困难很多。

    熵不够大时，攻击者可以进行暴力破解攻击，为抵御暴力破解攻击，可以在程序崩溃次数达到一定数量后，在一段时间内禁止该程序再次被执行，如grsecurity。

18. **IDE**：集成开发环境（IDE，Integrated Development Environment ）是用于提供程序开发环境的应用程序，一般包括代码编辑器、编译器、调试器和图形用户界面等工具。集成了代码编写功能、分析功能、编译功能、调试功能等一体化的开发软件服务套。

19. **GS寄存器**：指向的内存段是一个特殊的区域，不同于栈、堆、BSS段、数据段和代码段，重要的是，GS段与栈物理隔离，所以不会受堆栈的缓冲区溢出影响。

20. **使用gdb调试时一定要加上-g！！！**

21. **sh脚本或perl脚本，shell语言**

22. **stack smashing detected**

23. **Segmentation fault**

    

    

## 习题

## 实验

###  Turning Off Countermeasures

1. Address Space Randomization. `sudo sysctl -w kernel.randomize_va_space=0`

2. The StackGuard Protection Scheme. `gcc -fno-stack-protector example.c`

3. Non-Executable Stack

   ```shell
   For executable stack:
   $ gcc -z execstack  -o test test.c
   For non-executable stack:
   $ gcc -z noexecstack  -o test test.c
   ```

4. Configuring /bin/sh (Ubuntu 16.04 VM only). `$ sudo ln -sf /bin/zsh /bin/sh`



### Task 1: Running Shellcode

> ​		在开始攻击之前，让我们先熟悉shellcode。shellcode是启动shell的代码。它必须被加载到内存中，这样我们才能迫使易受攻击的程序跳转到它。考虑下面的程序：
>
> ```c
> #include <stdio.h>
> 
> int main() {
> 	char*name[2];
>     
> 	name[0] = "/bin/sh";
> 	name[1] = NULL;
> 	execve(name[0], name, NULL);
> }
> ```
>
> ​		我们使用的shellcode只是上述程序的汇编版本。下面的程序演示如何通过执行存储在缓冲区中的shellcode来启动shell。请编译并运行以下代码，并查看是否调用了shell。你可以从网站上下载这个程序。如果您有兴趣编写自己的外壳代码，我们有一个单独的SEED lab。

![image-20211010134155223](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010134155223.png)

关闭了不可执行栈，成功调用了shell。

### The Vulnerable Program

![image-20211010135213367](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010135213367.png)

### Task 2: Exploiting the Vulnerability

> ​		我们为您提供一个部分完成的 exploit代码，称为“exploit.c”。这段代码的目标是为“badfile”构造内容。在这个代码中，shellcode是给你的。你需要开发剩下的部分。
>
> ​		完成上述程序后，编译并运行它。这将生成badfile的内容。然后运行易受攻击的程序堆栈。如果正确实现了利用漏洞，您应该能够获得根shell：

使用python生成shellcode，填充代码为：

![image-20211010171742895](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010171742895.png)

使用c语言生shellcode，填充代码为，这里要注意小端对齐：

![image-20211010171959118](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010171959118.png)

运行结果为：

![image-20211010171643168](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010171643168.png)

### Task 3: Defeating dash’s Countermeasure

> ​		正如我们之前所解释的，Ubuntu 16.04中的dash shell检测到有效UID不等于实际UID时，将放弃特权。这可以从dash程序的变更日志中观察到。我们可以看到另一个签入行①，它比较真实和有效的用户/组ID。
>
> ```c
> // https://launchpadlibrarian.net/240241543/dash_0.5.8-2.1ubuntu2.diff.gz
> // main() function in main.c has following changes:
> ++ uid = getuid();
> ++ gid = getgid();
> 
> ++ /*
> ++
> ++  * To limit bogus system(3) or popen(3) calls in setuid binaries,
> ++  * require -p flag to work in this situation.
> ++	*/
> ++ if (!pflag && (uid != geteuid() || gid != getegid())) { // ①
> ++  	setuid(uid);
> ++  	setgid(gid);
> ++  	/*PS1 might need to be changed accordingly.*/
> ++  	choose_ps1();
> ++ }
> ```
>
> ​		在dash中实施的对策可能会失败。一种方法是不在外壳代码中调用/bin/sh；相反，我们可以调用另一个shell程序。这种方法要求系统中存在另一个shell程序，如zsh。另一种方法是在调用dash程序之前将受害者进程的实际用户ID更改为零。我们可以通过在外壳代码中执行execve（）之前调用setuid（0）来实现这一点。在本任务中，我们将使用这种方法。我们将首先更改/bin/sh符号链接，使其指向/bin/dash：
>
> ```shell
> $ sudo ln -sf /bin/dash /bin/sh
> ```
>
> ​		为了了解dash中的对策是如何工作的，以及如何使用系统调用setuid（0）来击败它，我们编写了以下C程序。我们首先注释掉第①行，并将该程序作为Set-UID程序运行（所有者应该是root）；请描述你的观察结果。然后我们取消注释行①并再次运行程序；请描述你的观察结果。
>
> ```c
>  // dash_shell_test.c
> 
>  #include <stdio.h>
>  #include <sys/types.h>
>  #include <unistd.h>
>  int main()
>  {
>      char*argv[2];
>      argv[0] = "/bin/sh";
>      argv[1] = NULL;
>      
>      // setuid(0); // ①
>      execve("/bin/sh", argv, NULL);
>      
>      return 0;
>  }
> 
> 
> ```

注释掉setuid(0)且shell为zsh时可以进入root shell，此时有效用户是root，真实用户是seed

![image-20211010173013640](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010173013640.png)

注释掉setuid(0)，但修改为dash时只能进入普通的shell

![image-20211010173243298](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010173243298.png)

取消注释setuid(0)，此时进入root shell，且真实用户id为root

![image-20211010173501278](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010173501278.png)

在shellcode的开头加上setuid(0)后，即使在shell为dash的情况下，仍旧可以攻击成功

![image-20211010173823715](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010173823715.png)

### Task 4: Defeating Address Randomization

> 在32位Linux机器上，堆栈只有19位的熵，这意味着堆栈基址可以有$2^{19} = 524288$的可能性。这个数字并不是很高，用蛮力的方法很容易耗尽。在本任务中，我们使用这种方法来消除32位虚拟机上的地址随机化对策。首先，我们使用以下命令打开Ubuntu的地址随机化。我们运行任务2中开发的相同攻击。请描述并解释你的观察结果。
>
> ```sh
> sudo /sbin/sysctl -w kernel.randomize_va_space=2
> ```
>
> 然后，我们使用暴力方法反复攻击易受攻击的程序，希望我们在坏文件中输入的地址最终是正确的。您可以使用以下shell脚本在无限循环中运行易受攻击的程序。如果您的攻击成功，脚本将停止；否则，它将继续运行。请耐心等待，因为这可能需要一段时间。如果需要，让它通宵运行。请描述你的观察结果。
>
> ```shell
> #!/bin/bash
> 
> SECONDS=0
> value=0
> 
> while [ 1 ]
>     do
>     value=$(( $value + 1 ))
>     duration=$SECONDS
>     min=$(($duration / 60))
>     sec=$(($duration % 60))
>     echo "$min minutes and $sec seconds elapsed."
>     echo "The program has been running $value times so far."
>     ./stack
> done
> ```

修改地址随机化变量为2后，执行以上脚本，在运行了9209次后，成功获得root shell

![image-20211010175058142](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010175058142.png)

### 任务5：打开StackGuard保护

> ​		在执行此任务之前，请记住先关闭地址随机化，否则您将不知道哪种保护有助于实现保护。
>
> ​		在前面的任务中，我们在编译程序时禁用了GCC中的StackGuard保护机制。在这个任务中，您可以考虑在StackGuard的存在下重复任务2。为此，您应该在编译程序时不使用-fno-stack-protector选项。对于此任务，您将重新编译易受攻击的程序stack.c、 要使用GCC StackGuard，请再次执行任务1，并报告您的观察结果。您可以报告您观察到的任何错误消息。
>
> ​		在GCC版本4.3.3及以上，默认情况下启用StackGuard。因此，必须使用前面提到的开关禁用StackGuard。在早期版本中，它在默认情况下被禁用。如果使用较旧的GCC版本，则可能不必禁用StackGuard。

关闭地址随机化，关闭不可执行栈，打开StackGuard，重新运行task2，攻击失败，stack smashing detected

![image-20211010180414992](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010180414992.png)

### Task 6: Turn on the Non-executable Stack Protection

> ​		在执行此任务之前，请记住先关闭地址随机化，否则您将不知道哪种保护有助于实现保护。
>
> ​		在前面的任务中，我们有意使堆栈可执行。在本任务中，我们使用noexecstack选项重新编译易受攻击的程序，并重复任务2中的攻击。你能得到一个shell吗？如果没有，问题是什么？此保护方案如何使您的攻击变得困难。你应该在实验室报告中描述你的观察结果和解释。您可以使用以下说明来启用不可执行的堆栈保护。
>
> ```sh
> gcc -o stack -fno-stack-protector -z noexecstack stack.c
> ```
>
> ​		需要注意的是，非可执行堆栈只会导致无法在堆栈上运行shellcode，但不能防止缓冲区溢出攻击，因为在利用缓冲区溢出漏洞后，还有其他方法可以运行恶意代码。return-to-libc攻击就是一个例子。我们为那次袭击设计了一个单独的实验室。如果您感兴趣，请参阅我们return-to-libc攻击实验室以了解详细信息。
>
> ​		如果你正在使用我们的Ubuntu 12.04/16.04 VM，不可执行堆栈保护是否有效取决于CPU和虚拟机的设置，因为此保护取决于CPU提供的硬件功能。如果您发现不可执行堆栈保护不起作用，请查看链接到实验室网页的文档（“不可执行堆栈上的注释”），并查看文档中的说明是否有助于解决您的问题。如果没有，那么您可能需要自己解决问题。

在关闭地址随机化，关闭StackGuard，开启不可执行堆栈的情况下，攻击失败，Segmentation fault，触发段错误**[question0]**什么是段错误

![image-20211010180902639](C:\Users\13990\AppData\Roaming\Typora\typora-user-images\image-20211010180902639.png)
