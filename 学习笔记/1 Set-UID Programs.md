# Set-UID Programs

## 知识点

1. 进程的三个ID：

   **真实用户ID**（实际用户ID，real user id）：RUID（UID）进程的执行者，唯一确定

   **有效用户ID**（effective user id）：EUID 进程执行时对文件的访问权限

   **保留用户ID**（saved set-user-id）：SSUID 为EUID的副本，执行exec调用时后能重新恢复

   <u>进程权限应该是真实用户ID和有效用户ID共同决定的。</u>

2. **组ID**（group id）：GID

3. 设置用户ID（set-user-id）：SUID 文件特有的概念。

   setUID的定义：当一个可执行程序具有setUID权限，用户执行这个程序时，将以这个程序所有者的身份执行。

   当set-UID程序运行时，进程会得到root权限

4. chmod（**change mode**）命令是控制用户对文件的权限的命令，若用 `chmod 4755 filename` 

   或 `chmod u+s` 可使此程序具有 root 的权限，同时执行时也需要拥有root权限，其中的4设置了set-UID比特。

   ![https://www.runoob.com/wp-content/uploads/2014/08/rwx-standard-unix-permission-bits.png](https://www.runoob.com/wp-content/uploads/2014/08/rwx-standard-unix-permission-bits.png)

5. chown（**change owner**）命令用于设置文件所有者和文件关联组的命令。

   chown 需要超级用户 root 的权限才能执行此命令。

   chown会自动清空设置的Set-UID比特，所以需要重新运行chmod指令设置set-UID比特。

6. **setuid函数**：setuid()用来重新设置执行目前进程的用户ID。不过，要让此函数有作用，其有效用户ID必须为0(root)。如果当前的有效用户ID不是root，则此函数行为与seteuid函数一致，就是把有效用户ID设置为uid参数；反之，则会吧真实用户ID和保存用户ID一并设置，设置后这个进程就不再是set-UID程序了，因为三个ID一致了。

   **seteuid函数**：seteuid()用来重新设置执行目前进程的有效用户ID。如果当前有效用户ID是root，则uid参数可以是任何值；反之则uid参数只能是有效用户ID、真实用户ID或者保存的用户ID。**[question]**

   **setreuid函数**：来设定真实用户ID和有效用户ID。这个函数在由有效用户ID为0的进程调用时，不会改变SSUID。

   getenv函数：getenv（）函数是个获取变量的函数。

7. | 文件属性     | 文件数 | 拥有者 | 所属group | 文件大小 | 建档日期     | 文件名 |
   | ------------ | ------ | ------ | --------- | -------- | ------------ | ------ |
   | -rwx--x--x   | 1      | root   | root      | 89080    | Nov 7 22:41  | tar*   |
   | -rwsr--xr--x | 1      | root   | seed      | 7275     | Feb 23 09:41 | catall |

8. ```bash
   find / -perm -04000 -type f -ls
   ```

   列出根目录下所有suid程序

   ```bash
   find / -type f ( -perm -4000 -o -perm -2000 ) –print
   ```

   查看系统中有哪些 SUID 和 SGID文件

9. **启动新进程（system函数）**：可以启动一个新的进程，它必须用一个shell来启动需要的程序。

   **替换进程映像（exec函数）**：可以用来替换进程映像，执行exec系列函数后，原来的进程将不再执行，新的进程的PID、PPID和nice值与原先的完全一样。

   execve()执行外部命令，接收参数为：①运行的指令；②指令用到的参数；③传入新程序的环境变量。

   **复制进程映像（fork函数）**：可以通过调用fork创建一个新进程。这个系统调用复制当前进程，在进程表中新建一个新的表项，新表项中的许多属性与当前进程是相同的。新进程几乎与元进程一模一样，执行的代码也完全相同，但是新进程有自己的数据空间、环境和文件描述符。

   **system()、exec()、fork()函数比较**：

   - 执行exec函数后，并没有产生新的进程，但将进程空间换掉了，新的进程空间是为了执行新的程序所准备的。

   - 执行fork()函数后，产生了新的进程，新的进程的PID、PPID与原来原来的进程不同，说明父子进程是两个不同的进程，不再共享任何数据。但fork刚执行后的一段时间内，父子进程有着相同的状态（进程空间中的东西都一样，因为fork采用“写时复制”，一开始父子进程共享物理内存空间），这将一直持续到子进程开始运行。

   - system()函数实际上就是先执行了fork函数，然后新产生的子进程立刻执行了exec函数。system函数会产生新进程，这就意味着新进程的PID、PPID等与原进程不同。system也会产生新的进程空间，而且新的进程空间是为新的程序准备的，所以和原进程的进程空间没有任何关系。 

   - Unix下某个进程的内存分成**三部分**：代码段，堆栈段，数据段。代码段用来存放程序运行的代码，堆栈段用来存放子程序的局部变量，数据段用来存放全局变量。

     fork中，父进程将代码段，堆栈段，数据段完全复制一份给子进程，然而，一旦子进程开始运行，它的数据段和堆栈段就在内存里完全和父进程分离开了。

     exec中，一旦调用exec类函数，它本身就“死亡”了，系统把代码段替换成新的程序的代码，废弃原有的数据段和堆栈段，并为新程序分配新的数据段与堆栈段，唯一留下的，就是进程号。

10. **文件描述符**：内核为了高效管理已被打开的文件所创建的索引，用于指向被打开的文件，所有执行I/O操作的系统调用都通过文件描述符；文件描述符是一个简单的非负整数，用以表明每个被进程打开的文件。

11. 

    

    

## 习题

###### Alice运行由Bob拥有的Set-UID程序。程序试图读取从/tmp/x，这对Alice是可读的，但对其他人不是。这个程序可以成功读取文件?

```bash
可以，Set-UID让Bob以root身份访问/tmp/x，而root拥有最高权限，所有用户的文件均可被它读写
```

###### 进程试图打开一个文件进行读取。进程的有效用户ID是1000，实际用户ID为2000。该文件对用户ID 2000是可读的，但对用户ID 1000不是。可以这进程成功打开文件?

```bash
实际用户ID为2000，且文件对用户ID2000可读，那么即可成功打开文件，无需关心是不是对ID1000可读
```

###### root拥有的Set UID程序允许普通用户在执行程序。是什么阻止用户使用特权做坏事？

最小权限原则，允许普通用户执行的程序，仍然是存在限制的，不是所有程序都可执行，如同手机的定位权限，存储权限，照相权限等，细粒度管理

###### 我们正在尝试将seed用户拥有的程序prog转换为Set UID程序属于root的。运行以下命令可以达到目标吗？

```shell
$ sudo chmod 4755 prog
$ sudo chown root prog
```

```shell
答：不可以，因为chown会自动清空设置的Set-UID比特，应该先'chown'，后'chmod'设置比特
$ sudo chown root prog
$ sudo chmod 4755 prog
```

###### chown命令自动禁用Set-UID位，当它改变Set-UID程序的所有者,请解释一下为什么会这样?

```bash
chown命令有时会清除set-user-ID或set-group-ID权限位。这种行为取决于底层系统调用'chown'的
策略和功能，它可能会在'chown'命令控制之外进行依赖于系统的文件模式修改。
例如，当具有适当特权的用户调用'chown'命令时，或者当这些位表示一些功能而不是可执行权限（强制锁定）时，可能不会影响这些位。当有疑问时，检查底层系统行为。
```

###### 当我们调试一个程序时，我们可以在执行过程中改变程序的局部变量。这可以改变程序的行为。我们可以使用这种技术来调试Set-UID程序并改变它的行为吗?例如，如果程序应该打开/tmp/xyz文件，我们可以修改文件名字符串，因此Set-UID程序最终会打开/etc/passwd.

```php
如果程序直接拼接了普通用户传入的参数（如：路径）拼接命令未做任何过滤并直接执行，上述方式是可行的
cin<<getParam;
exec("vi " + getParam);
```

###### system()和execve()都可以用来执行外部程序。为什么system()不安全，而execve()是安全的?

```bash
system()在使用过程中，如果不对传入参数进行过滤，很容易导致拼接传入的命令，继而执行用户自定义命令。
而execve()是参数化的，传入的命令参数只会被当作一个命令来处理
如：
传入恶意指令getParam=aaa;/bin/bash：
cin<<getParam;
system("vi" +getParam)
最终执行的是两个语句，vi aaa和 /bin/bash

而
cin<<getParam;
execve("vi" +getParam,arg1,0)
最终执行的是一个语句，vi 'aaa;/bin/bash'
```

###### 当程序从用户那里接受输入时，我们可以重定向输入设备，这样程序就可以从文件中接受输入。例如，我们可以使用prog < myfile来提供myfile中的数据作为prog程序的输入。现在，如果prog是root用户拥有的Set-UID程序，我们是否可以使用以下方法来获取这个特权程序，以从/etc/shadow文件中读取?[question]

```shell
$ prog < /etc/shadow
```

```bash
拥有Set-UID的prog程序没有对参数进行校验，直接输出用户传入的文件路径，输出/etc/shadow
```

###### 当父Set-UID进程(有效用户ID为root，真实用户ID为bob)使用fork()创建子进程时，父进程的标准输入、输出和错误设备将被子进程继承。如果子进程放弃了它的root权限，它仍然保留对这些设备的访问权限。这似乎是一个功能泄漏，类似于本章所述的功能泄漏案例。这有什么危险吗?

```
会造成权限泄漏。当特权程序打开文件后忘记关闭，此时回到普通身份，文件描述符是仍然有效的，因此这个非特权程序仍然可以修改打开的文件
```

###### 超级用户想给Alice使用more命令查看系统中所有文件的权限。他做了以下工作:

```powershell
cp /bin/more /tmp/mymore
$ sudo chown root /tmp/mymore
$ sudo chmod 4700 /tmp/mymore
```

基本上，上面的命令将`/tmp/mymore`转换为Set-UID程序。现在，因为权限被设置为4700，其他用户不能执行该程序。超级用户使用另一个命令(现在显示)只将执行权限授予Alice。我们并不是假设Alice是完全被信任的。如果Alice只能读取其他4人的文件，这是可以的，但如果Alice可以获得任何特权，如写入其他人的文件，这是不可以的。请阅读更多程序的手册，找出爱丽丝可以做什么来获得更多的特权。

```
more指令是可以调用vi的，如果more具有set-uid权限，那么调用vi同样具有set-uid权限，vi又可以执行系统命令，所以Alice可直接利用此方式以set-uid权限执行任意系统命令
```

## 实验

### Task 1

> ​		在这个任务中，我们研究可以用来设置和取消设置环境变的命令。我们在seed帐户使用Bash 。用户使用默认 shell设置在/etc/passwd文件（每个条目的最后一个字段）中。您可以使用chsh 命令将其更改为另一个 Shell程序（请不要在实验室中这样做）。请做以下工作：

- 使用printenv或env查看所有环境变量，使用`printenv variables`或`env | grep variables`查看具体环境变量；

  ![image-20211011130347882](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130347882.png)

  env查看具体环境变量时会同时显示当前变量值和旧环境变量值。

- 使用export设置环境变量，使用unset取消设置环境变量。需要注意的是，这两个命令并不是独立的程序；它们是 Bash 的两个内部命令（您将无法在 Bash 之外找到它们）。

  ![image-20211011130358136](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130358136.png)

  观察发现，unset会取消当前的环境变量，所以在printenv中没有显示，而在env中只有旧环境变量值的显示。

  ![image-20211011130405967](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130405967.png)

  观察发现，PWD和OLDPWD是两个不同的环境变量。

### Task 2

> ​		在这个任务中，我们研究环境变量是如何被子进程从它们的父进程继承的。在Unix中，fork()通过复制调用进程来创建一个新进程。新进程，称为子进程，是调用进程的精确副本，称为父进程;但是，有一些东西不是由子节点继承的（请参阅fork()手册，输入以下命令:`man fork`）。在这个任务中，我们想知道父进程的环境变量是否被子进程继承。

1. 将环境变量从父进程传递到子进程

   - pid_t  其实就是 int 类型.

   - 子进程是父进程的副本，它将获得父进程的环境变量、数据空间、堆、栈等资源的副本。但是，子进程持有的是上述存储空间的“副本”，这意味着父子进程间不共享这些存储空间，它们之间共享的存储空间只有代码段。

     ![image-20211011130418994](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130418994.png)

2. <u>子进程不继承父进程进程号，他们的进程号不同；他们有不同的父进程号；子进程有自己的文件描述符和目录流的拷贝，子进程不继承父进程的进程，正文(text)， 数据和其它锁定内存等。</u>

3. 没有对比出区别，说明环境变量一致，子进程继承了父进程的所有的环境变量。

   ![image-20211011130427109](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130427109.png)

### Task 3

> ​		在这个任务中，我们研究当通过execve()执行新程序时，环境变量是如何受到影响的。execve()函数调用一个系统调用来加载一个新命令并执行它；这个函数永远不会重新出现转弯。没有创建新的进程；相反，调用进程的文本、数据、bss和堆栈被加载的程序覆盖。实际上，execve()在调用过程中运行新程序。我们感兴趣的是环境变量的变化；它们会自动被新程序继承吗?

- **[error]**发现执行错误，关于execve()函数的，使用man execve查看帮助文档后，发现示例代码中没有包含该有的头文件unistd.h。

1. 传递给执行文件的新环境变量数组为NULL，所以没有新的环境变量。

   ![image-20211011130434335](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130434335.png)

2. 传递了环境变量过去，所以有结果显示。

   ![image-20211011130446179](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130446179.png)

3. execve()用来执行参数filename字符串所代表的文件路径，第二个参数是利用数组指针来传递给执行文件，并且需要以空指针(NULL)结束，最后一个参数则为传递给执行文件的新环境变量数组。

   新程序通过第三个参数获得环境变量数组。且调用execve（）之后，会转到新的进程，自己会被“杀死”，也就是execve（）函数之后代码都不会被执行。

   ![image-20211011130453108](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130453108.png)

### Task 4

> ​		在这个任务中，我们研究当通过system()函数执行一个新程序时，环境变量是如何受到影响的。此函数用于执行命令，但与直接执行命令的execve()不同，system()实际上执行`/bin/sh -c command`，即，它执行`/bin/sh`，并请求shell执行该命令。
>
> ​		如果您查看system()函数的实现，您将看到它使用execl()来执行`/bin/sh`;  excel()调用execve()，将环境变量数组传递给它。因此，使用system()，调用过程的环境变量被传递给新的程序`/bin/sh`。请编译并运行以下程序来验证这一点。

**[question]**运行结果为环境变量，不知道如何进行验证。

但通过`man system`可以看到system()函数的具体执行过程。实际上system（）函数执行了三步操作：fork一个子进程；在子进程中调用exec函数去执行command； 在父进程中调用wait去等待子进程结束。

![image-20211011130503437](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130503437.png)

### Task 5

> ​		Set-UID是Unix操作系统中的一种重要的安全机制。当Set-UID程序运行时，它会接受所有者的特权。例如，如果程序的所有者是root,那么当任何人运行这个程序时，程序就会在执行期间获得root的特权。Set-UID允许我们做许多有趣的事情，但它在执行时提升了用户的特权，使其相当危险。虽然Set-UID程序的行为是由程序逻辑决定的，而不是由用户决定的，但用户确实可以通过环境变量影响行为。为了理解Set-UID程序是如何受到影响的，让我们首先确定Set-UID程序的进程是否从用户的进程继承了环境变量。

1. 正常打印出环境变量

2. 修改了可执行文件t5.out的所属权为root，并设置为set-UID程序

   ![image-20211011130511618](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130511618.png)

3. 首先设置变量

   ![image-20211011130521337](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130521337.png)

   然后运行步骤2的set-UID程序

   ![image-20211011130531668](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130531668.png)

   **[question]**很奇怪的是，环境变量LD_LIBRARY_PATH存在于父进程中，但在子进程中找不到。同时发现修改这个变量之前，即使是在父进程中，在环境变量列表中也没有这个变量。

### Task 6

> ​		由于调用了shell程序，在Set-UID程序中 调用system()非常危险。这是因为shell程序的实际行为可能受到环境变量(如IPATH)的影响;这些环境变量由用户提供，可能是恶意的。通过更改这些变量，恶意用户可以控制Set-UID程序的行为。在Bash中，您可以用以下方式更改PATH环境变量(本示例将目录/home/seed添加到PATH环境变量的开头):
>
> ```bash
>  $ export PATH=/home/seed:$PATH
> ```

将t6.out的权限修改为root后，直接执行发现执行的命令并不是`ls`，而是`/bin/ls`

![image-20211011130540812](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130540812.png)

查看SHELL的环境变量值，发现为/bin/bash。

![image-20211011130547973](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130547973.png)

由于system（）函数是调用了shell环境变量。于是尝试将自己的可执行文件夹所在的目录加在了SHELL环境变量的开头：从而使这个Set-UID程序运行我的代码而不是`/bin/ls`，但是发现只修改环境变量并不可行，因此采用了书上的方法，将SHELL链接到了没有防护的zsh上，成功执行了我的指令。

**[question]**bash和dash是不同的，环境变量中的SHELL的值为bash，而据书中所说默认链接的SHELL是dash，有些疑惑，环境变量中的值到底是什么意义呢

![image-20211011130556837](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130556837.png)



后续想明白了实验的目的，另外写了一个ls.c程序，其中代码为`system("./t5.out");`，并且修改`export PATH=.:$PATH`，然后运行t6.out成功运行了t5.out。



### Task 7

> ​		在本任务中，我们将研究Set-UID程序如何处理一些环境变量。几个环境变量，包括LD_PRELOAD、LD_LIBRARY_PATH和其他LD_*，影响行为动态加载器/链接器。动态加载器/链接器是操作系统(OS)的一部分（用于从持久性存储到RAM），并在运行时链接可执行文件所需的共享库。
>   在Linux中，ld.so或ld-linux。动态加载器/链接器也是如此(每种加载器对应不同类型的二进制文件)。在影响它们行为的环境变量中LD_PRELOAD和LD_LIBRARY_PATH是我们在实验室里关注的两个。在Linux中，LD_LIBRARY_PATH是一个执行搜索的目录，它应该在标准目录之前首先搜索。LD_PRELOAD指在所有其他库之前加载的用户指定的其他共享库列表。在这项任务中，我们只会研究LD_PRELOAD。

**[question]**`gcc -fPIC -g -c mylib.c``gcc -shared -o libmylib.so.1.0.1 mylib.o -lc`

1. 过程与书上类似

   ![image-20211011130604165](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130604165.png)

2. 当是set-UID程序，且使用root用户运行时，系统会忽略LD_PRELOAD环境变量，所以调用的sleep()是系统的函数，在其他情况则调用的是自行定义的共享库中的函数。

   ![image-20211011130612783](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130612783.png)

   但换成普通用户的set-UID程序运行时，则会调用我们定义的函数。

   ![image-20211011130620795](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130620795.png)

   但换成第三个用户依旧不行

3. 子进程，和父进程同时调用sleep函数，表现不同，证明是环境变量的影响

   ```c
   # fork函数
   #include <unistd.h>
   #include <stdio.h>
   
   int main()
   {
    	printf("main sleep:\n");
   	sleep(1);
           if(0 == fork()){
   	    printf("fork sleep:\n");
               sleep(1);
           }
           return 0;
   }
   ```

   ```c
   # 不传环境变量的exec函数
   #include <unistd.h>
   #include <stdio.h>
   
   extern char** environ;
   int main()
   {
    	printf("main sleep:\n");
   	sleep(1);
   
   	char *argv[2]={NULL};
   	printf("execve(NULL) sleep:\n");
       execve("myprog", argv, NULL);
       return 0;
   }
   ```

   ```c
   # 传递参数的execve
   #include <unistd.h>
   #include <stdio.h>
   
   extern char** environ;
   int main()
   {
    	printf("main sleep:\n");
   	sleep(1);
   
   	char *argv[2]={NULL};
   	printf("execve(NULL) sleep:\n");
      execve("myprog", argv, environ);
      return 0;
   }
   ```

   结果显示第一个和第三个的运行结果为父进程和子进程调用了同样的自定义的sleep()函数，说明确实是是否继承环境变量影响的。

### Task 8

> ​		虽然system()和lexecve()都可以用来运行新程序，但是如果在特权程序(如Set-UID程序)中使用，那么system()就非常危险。我们已经看到PATH环境变量如何影响system()的行为，因为变量影响shell的工作方式。execve()没有问题，因为它不调用shell。调用shell还有另一一个危险的后果，这一次，它与环境变量无关。让我们看看下面的场景。
>   Bob在一家审计机构工作，他需要调查一家公司涉嫌欺诈。为了调查目的，Bob需要能够读取公司Unix系统中的所有文件；另一方面，为了保护系统的完整性，Bob不应该修改任何文件。为了实现这个目标，系统的超级用户Vince编写了-一个特殊的set-root-uid程序(见下文)，然后将可执行权限授予Bob。这个程序需要Bob在命令行中键入文件名，然后运行`/bin/cat`来显示指定的文件。由于程序以根目录运行，所以它可以显示Bob指定的任何文件。然而，由于程序没有写操作，Vince非常确定Bob不能使用这个特殊程序修改任何文件。

1. 尝试删除，成功

   ![image-20211011130638754](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130638754.png)

   **[question]**为什么v[0]是`/bin/cat`时运行`./t8.out ./test.c`时显示权限不够，必须要加上sudo才可以成功运行呢？

2. 切换之后，攻击无效了

   ![image-20211011130647145](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130647145.png)

   **[question]** 测试发现，同样的`./t8.out ./test.c`当system()运行时没有权限，但exec时就可以

   ![image-20211011130654335](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130654335.png)

   

### Task 9

>   为了遵循最小特权的原则，如果不再需要这些特权，Set-UID程序通常会永久地放弃它们的根特权。此外，有时程序需要将其控制权移交给用户;在这种情况下，必须撤销根特权。setuid()系统调用可用于撤消特权。根据手册，“setuid()设置调用进程的有效用户ID。如果调用者的有效UID是根，那么真正的UID和保存的集用户id也会被设置”。因此，如果具有有效UID0的set-UID程序调用setuid(n)，则该进程将成为-一个正常进程，其所有UID都被没置为n。
>   撤消特权时，常见的错误之一-是功能泄漏。这个过程可能在它仍然享有特权的时候获得了一-些特权能力;当特权被降级时，如果程序不清理这些功能，它们仍然可以被非特权进程访问。换句话说，尽管流程的有效用户ID不再具有特权，但流程仍然具有特权，因为它具有特权功能。编译以下程序，将其所有者更改为root，并将其设置为Set-UID程序。以普通用户的身份运行程序，并描述您所观察到的情况。文件/etc/zzz会被修改吗?请解释你的观察。



设置为set-UID程序并用root用户运行时，成功修改文件，但是不授权时显示无法打开文件。

![image-20211011130701392](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130701392.png)

![image-20211011130708697](D:\git_repository\computer_security\学习笔记\1 Set-UID Programs.assets\image-20211011130708697.png)

成功读取文件后，通过`setuid(getuid());`放弃root权限，但由于文件没有关闭导致权限泄露，所以可以以普通用户的身份修改文件。

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void main(){ 
	int fd;
	/* Assume that /etc/zzz is an important system file,
	* and it is owned by root with permission 0644.
	* Before running this program, you should creat
	* the file /etc/zzz first. */
	fd = open("/etc/zzz", O_RDWR | O_APPEND);

	if (fd == -1) {
		printf("Cannot open /etc/zzz\n");
		exit(0);
	}

	/* Simulate the tasks conducted by the program */
	sleep(1);

	/* After the task, the root privileges are no longer needed,
	it’s time to relinquish the root privileges permanently. */
	setuid(getuid()); /* getuid() returns the real uid */

	if (fork()) { /* In the parent process */
		close (fd);
		exit(0);
	} else { /* in the child process */
		/* Now, assume that the child process is compromised, malicious
		attackers have injected the following statements
		into this process */
		write (fd, "Malicious Data\n", 15);
		close (fd);
	}
}
```

**[question]** `if (fork())`是怎么判断父进程还是子进程的？

