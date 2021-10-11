# Shellshock

## 知识点

1.  **declare命令**：
   - 第一种用法，用来声明变量并设置变量的属性([rix]即为变量的属性），`declare [+/-][rxi][变量名称＝设置值]`
     - +/- 　"-"可用来指定变量的属性，"+"则是取消变量所设的属性。
     - -f 　仅显示函数。
     - r 　将变量设置为只读。
     - x 　指定的变量会成为环境变量，可供shell以外的程序来使用。
     - i 　[设置值]可以是数值，字符串或运算式。
   - 第二种用法，显示shell函数，`declare -f`
   - 若不加上任何参数，则会显示全部的shell变量与函数(与执行set指令的效果相同)
2.  **unset指令**：为shell内建指令，可删除变量或函数。
3.  **[question]**为什么真实用户ID与有效用户ID不同，则bash不会处理从环境变量处获得的函数定义，因此不会受到shellshock攻击？
4.  **命令行工具curl**：
5.  **PHP语言**：
6.  `env foo='() { echo "hello world";}; echo vulnerable;' bash_shellshock -c "echo this is a test"`
7.  **向子进程传递一个函数**，如果在父进程中直接定义函数的话，想传入子进程必须在父进程也是shell的情况下；而定义环境变量传递的方式则不要求父进程必须为shell。
8.  **使用shell去运行脚本**，两种方法：
    - bash -c "cmd string" 接的是shell命令字符串，用双引号括起来
    - bash -c  "/path/to/file" 写文件的绝对路径，用双引号括起来，且文件要具有可执行权限
9.  

## 习题

## 实验

### Task 1: Experimenting with Bash Function

> ​		Ubuntu 16.04中的Bash程序已经被修补，所以它不再容易受到shellshock攻击。出于本实验室的目的，我们在/bin文件夹中安装了一个易受攻击的Bash版本；它的名字叫bash——shellshock。我们需要在任务中使用这个Bash。请像下面这样运行此易受攻击的Bash版本，然后设计一个实验来验证此Bash是否易受Shellshock攻击的攻击。.
>
> `$ /bin/bash_shellshock`
>
> 在修补版本的bash（/bin/bash）上尝试相同的实验，并报告您的观察结果。

定义环境变量foo，值为看上去是函数定义的字符串，并且函数结尾的大括号外添加一个额外的打印命令。通过export标记该shell变量，并将他传至子进程，分别在子进程启动正常的bash和有漏洞的bash_shellshock，发现有漏洞的版本会正常启动且执行了额外命令，而正常版本无法启动。

![image-20211011130727929](D:\git_repository\computer_security\学习笔记\3 Shellshock.assets\image-20211011130727929.png)

**[question]**为什么定义完变量后，再export，想要传递给子进程函数，在运行bash时和运行bash_shellshock不一样呢？换言之shellshock被修复后是怎么样的？而且知识点6里运行后，函数也未定义啊？

![image-20211011130735541](D:\git_repository\computer_security\学习笔记\3 Shellshock.assets\image-20211011130735541.png)

### Task 2: Setting up CGI programs

> ​		在本实验室中，我们将对远程web服务器发起Shellshock攻击。许多web服务器都支持CGI，这是一种用于在web页面和web应用程序上生成动态内容的标准方法。许多CGI程序都是使用shell脚本编写的。因此，在执行CGI程序之前，将首先调用shell程序，并且这样的调用由来自远程计算机的用户触发。如果shell程序是易受攻击的Bash程序，我们可以利用Shellshock易受攻击来获得服务器上的权限。
>
> ​		在本任务中，我们将设置一个非常简单的CGI程序（称为myprog.CGI），如下所示。它只是使用一个shell脚本打印出“Hello World”。
>
> `#!/bin/bash_shellshock ` ①
> `echo "Content-type: text/plain"`
> `echo`
> `echo`
> `echo "Hello World"`
>
> 请确保在第①行中使用/bin/bash_shellshock，而不是使用/bin/bash。该行指定应该调用哪个shell程序来运行脚本。我们确实需要在此实验室中使用易受攻击的Bash。请将上述CGI程序放在/usr/lib/cgi-bin目录中，并将其权限设置为755（因此它是可执行的）。您需要使用root权限来执行这些操作，因为文件夹只能由root用户写入。此文件夹是Apache  web服务器的默认CGI目录。
>
> 要从Web访问此CGI程序，您可以通过键入以下URL使用浏览器：http://localhost/cgi-bin/myprog. 或使用以下命令行程序curl执行相同的操作：
>
> `$ curl http://localhost/cgi-bin/myprog.cgi`
>
> 在我们的设置中，我们从同一台计算机运行Web服务器和攻击，这就是我们使用localhost的原因。在实际攻击中，服务器运行在远程计算机上，我们使用服务器的主机名或IP地址，而不是使用localhost。





### Task 3: Passing Data to Bash via Environment Variable

> ​		要利用基于Bash的CGI程序中的Shellshock漏洞进行攻击，攻击者需要将其数据传递给易受攻击的Bash程序，并且需要通过环境变量传递数据。在这项任务中，我们需要看到如何实现这一目标。您可以使用以下CGI程序来演示您可以向CGI程序发送任意字符串，该字符串将显示在其中一个环境变量的内容中。
>
> `#!/bin/bash_shellshock`
> `echo "Content-type: text/plain"`
> `echo`
> `echo "******Environment Variables******"`
> `strings /proc/$$/environ`①
>
> ​		在上面的代码中，第①行打印出当前进程中所有环境变量的内容。如果您的实验成功，您应该能够在从服务器返回的页面中看到您的数据字符串。在您的报告中，请解释远程用户的数据如何进入这些环境变量。





### Task 4: Launching the Shellshock Attack

> ​		在上述CGI程序建立后，我们现在可以发动炮击攻击。攻击不依赖于CGI程序中的内容，因为它的目标是在执行CGI脚本之前首先调用的Bash程序。您的目标是通过URL发起攻击http://localhost/cgi-bin/ 我的节目。像cgi这样的远程用户做不到的事情。在本任务中，您应演示以下内容：
>
> - 使用Shellshock攻击从服务器窃取机密文件的内容
> - 回答以下问题：您是否能够窃取卷shadow文件`/etc/shadow`的内容？为什么？





### Task 5: Getting a Reverse Shell via Shellshock Attack

> ​		Shellshock漏洞允许攻击在目标计算机上运行任意命令。攻击者可以选择在shell中长时间运行命令，而不是在shell中运行命令。为了实现这一目标，攻击者需要运行反向shell。
>
> ​		反向shell是在机器上启动的shell进程，其输入和输出由远程计算机的某人控制。基本上，shell在受害者的机器上运行，但它从攻击者的机器上获取输入，并在攻击者的机器上打印输出。Reverse shell为攻击者提供了在受损机器上运行命令的方便方法。有关如何创建反向外壳的详细说明，请参见SEED book第3章（§3.4.5）。我们还将在后面的指南部分总结说明。
>
> ​		在本任务中，您需要演示如何通过CGI程序中的Shellshock漏洞启动reverse shell。请展示一下你是怎么做的。在您的报告中，还请解释如何设置反向shell，以及它为什么工作。基本上，你需要用你自己的话来解释reverse shell在你的shellshock中是如何工作的。





### Task 6: Using the Patched Bash

> 现在，让我们使用一个已经修补过的Bash程序。程序/bin/bash是一个修补版本。请用此程序替换CGI程序的第一行。重做任务3和5，并描述您的观察结果





### Guidelines: Creating Reverse Shell