# Race_Condition_new

## 知识点

1. **TOCTTOU**：检查与使用时差漏洞

2. **O_CREAT**标志：open() 系统调用的一个标志，其作用是当文件不存在时，根据文件名创建一个新文件并打开它。但其还有一个副作用是，当指定文件存在时，系统调用不会失败，他会以写权限打开该文件。

3. 限制一个程序是否可以使用一个全局可写目录中的符号链接，

   ```bash
   sudo sysctl -w fs.protected_symlinks=0
   ```

4. **fopen()** 实际上调用了open() 系统调用，因此它检查的是有效用户ID。 

5. **[question]**usleep() 函数对攻击的影响？尝试了很多不同的睡眠时间，始终没有攻击成功，总是在成功之前XYZ的拥有者变为了root。

6. 防御措施：

   - **原子操作**。把检查和使用操作原子化，从而消除检查和使用之间的时间窗口。一般是通过对原子上锁实现的，在检查和使用之间把目标文件锁上，但不是强制的，有利有弊，所以是软锁。
   - **重复检查和利用**。增加更多的竞态条件，使得竞争变得更加困难，攻击者只有全部赢得这些竞争才能成功。
   - **粘滞符号链接保护**。此保护机制只适用于人人可写的**粘滞（sticky）目录**，例如`/tmp`，此机制来防止程序在特定情况下跟随符号链接。通过知识点2中的命令关闭（Ubuntu16.04）。
   - **最小权限原则**：UNIX提供了两个系统调用`seteuid()` 和`setuid()`来让程序永久或暂时放弃其权限。

7. **粘滞（sticky）目录**：在Linux文件系统中，文件目录有一个特殊的比特位叫做粘滞位比特。当设置了这个比特位时，只有文件的所有者、目录所有者或者root用户才能重命名或者删除这个目录中的文件。如果没有设置粘滞位比特，任何具有目录写入和执行权限的用户都可以重命名或者删除文件。因为`/tmp`是全局可写的，所以为防止普通用户操作，设置了粘滞位比特。

   当**跟随者**和**符号链接所有者**或目录所有者一致时，才可以fopen成功。

8. 

## 习题

## 实验

### Initial Setup

> ​		Ubuntu 10.10及更高版本具有针对种族条件攻击的内置保护。此方案通过限制谁可以跟随符号链接来工作。根据文档，“如果跟随者和目录所有者与符号链接所有者不匹配，则无法跟踪世界可写粘性目录（例如/tmp）中的符号链接。”在本实验室中，我们需要禁用此保护。您可以使用以下命令来实现这一点：
>
> ```bash
> // On Ubuntu 12.04, use the following:
> sudo sysctl -w kernel.yama.protected_sticky_symlinks=0
> // On Ubuntu 16.04, use the following:
> sudo sysctl -w fs.protected_symlinks=0
> ```

### A Vulnerable Program

> ​		下面的程序是一个看似无害的程序。它包含竞争条件漏洞。
>
> ```c
> #include <unistd.h>
> #include <stdio.h>
> #include <string.h>
> 
> int main(){
> 	char *fn = "/tmp/XYZ";
> 	char buffer[60];
> 	FILE *fp;
> 
>     scanf("%50s", buffer);
> 
>     if(!access(fn, W_OK)){
>         fp = fopen(fn, "a+");				①
>         fwrite("\n", sizeof(char), 1, fp);	②
>         fwrite(buffer, sizeof(char), strlen(buffer), fp);
>         fclose(fp);
>     }else{
>         printf("No permission \n");
>     }
> 
>     return 0;
> 
> }
> ```
>
> ​		上面的程序是root拥有的Set-UID程序；它将用户输入字符串附加到临时文件`/tmp/XYZ`的末尾。由于代码以root权限运行，i.e它的有效使用ID为零，可以覆盖任何文件。为了防止自己意外覆盖他人的文件，程序首先检查真实用户ID是否具有文件`/tmp/XYZ`的访问权限；这就是第行中access() 调用的目的。如果实际用户ID确实拥有权限，程序将在第①行中打开文件，并将用户输入附加到文件中。
>
> ​		乍一看，这个程序似乎没有任何问题。但是，此程序中存在竞争条件漏洞：由于检查（access() ）和使用（fopen() ）之间存在时间窗口，access() 使用的文件可能与fopen() 使用的文件不同，即使它们具有相同的文件名`/tmp/XYZ`。如果恶意攻击者能够以某种方式使`/tmp/XYZ`变成指向受保护文件（如`/etc/passwd`）的符号链接，在时间窗口内，攻击者可以使用户输入附加到`/etc/passwd`，从而获得root权限。该漏洞以root权限运行，因此它可以覆盖任何文件。

注意临时文件/tmp/XYZ要用seed普通账户创建所属，如果/tmp/XYZ所属root是没有权限给它建立符号链接到其他文件的。

### Task 1: Choosing Our Target

> ​		我们希望利用易受攻击程序中的竞争条件漏洞。我们选择以密码文件`/etc/passwd`为目标，普通用户无法写入该文件。通过利用该漏洞，我们希望向密码文件添加一条记录，目的是创建一个具有root权限的新用户帐户。在密码文件中，每个用户都有一个条目，由七个字段组成，字段之间用冒号（:）分隔。根用户的条目如下所示。对于根用户，第三个字段（用户ID字段）的值为零。也就是说，当root用户登录时，其进程的用户ID设置为零，从而赋予该进程root权限。基本上，根帐户的功能不是来自其名称，而是来自用户ID字段。如果我们想创建一个具有root权限的帐户，我们只需要在这个字段中加一个零。
>
> ```
> root:x:0:0:root:/root:/bin/bash
> ```
>
> ​		每个条目还包含一个密码字段，这是第二个字段。在上面的示例中，该字段设置为“x”，表示密码存储在另一个名为`/etc/shadow`的文件（shadow文件）中。如果我们遵循这个示例，我们必须使用竞争条件漏洞来修改密码和shadow文件，这并不难做到。然而，有一个更简单的解决方案。我们可以简单地将密码放在那里，而不是将“x”放在密码文件中，这样操作系统就不会从shadow文件中查找密码。
>
> ​		密码字段不包含实际密码；它保存密码的单向散列值。为了获得给定密码的值，我们可以使用adduser命令在我们自己的系统中添加一个新用户，然后从影子文件中获取密码的单向散列值。或者我们可以简单地从seed用户的条目复制值，因为我们知道它的密码是dees。有趣的是，Ubuntu live CD中有一个用于无密码帐户的magic value，magic value是U6aMy0wojraho（第6个字符是零，不是字母O）。如果我们把这个值放在用户输入的密码字段中，我们只需要在提示输入密码时按回车键。
>
> ​		**TASK.** 为了验证magic密码是否有效，我们（作为超级用户）手动将以下条目添加到`/etc/passwd`文件的末尾。请报告您是否可以在不键入密码的情况下登录测试帐户，并检查您是否具有root权限。
>
> ```
> test:U6aMy0wojraho:0:0:test:/root:/bin/bash
> ```
>
> ​		完成此任务后，请从密码文件中删除此条目。在下一个任务中，我们需要以普通用户的身份实现这一目标。显然，我们不允许直接对密码文件执行此操作，但我们可以利用特权程序中的竞争条件来实现相同的目标。
>
> ​		**Warning.** 过去，一些学生在攻击过程中意外清空了`/etc/passwd`文件（这可能是由于操作系统内核中的某些竞争条件问题造成的）。如果密码文件丢失，您将无法再次登录。为避免此问题，请复制原始密码文件或拍摄虚拟机的快照。这样，你就可以很容易地从灾难中恢复过来.

### Task 2.A: Launching the Race Condition Attack

与书上完全一致



### Task 2.B: An Improved Attack Method 

> ​		在任务2.A中，如果您已经正确完成了所有操作，但仍然无法成功进行攻击，请检查`/tmp/XYZ`的所有权。您将发现`/tmp/XYZ`的所有者已成为root（通常，它应该是seed）。如果发生这种情况，您的攻击将永远不会成功，因为您的攻击程序以seed权限运行，无法再删除或unink() 。这是因为`/tmp`文件夹上有一个“粘滞”位，这意味着只有文件的所有者才能删除该文件，即使该文件夹是可写的。
>
> ​	在任务2.A中，我们允许您使用root的权限删除`/tmp/XYZ`，然后重试攻击。不希望出现的情况是随机发生的，因此通过重复攻击（在root的“帮助”下），您最终将成功完成任务2。显然，从root获得帮助并不是真正的攻击。我们想摆脱它，在没有root帮助的情况下完成它。
>
> ​		造成这种不良情况的主要原因是我们的攻击程序有一个问题，一个竞态条件问题，这正是我们试图在受害者程序中利用的问题。真讽刺！过去，当我们看到这个问题时，我们只是建议学生删除该文件并再次尝试攻击。感谢我的一个学生，他决心找出问题所在。由于他的努力，我们终于明白了原因，并找到了改进的解决方案。
>
> ​		**发生这种情况的主要原因是，攻击程序在删除`/tmp/XYZ`（即unlink() ）之后，但在将名称链接到另一个文件（即symlink() ）之前，上下文被关闭。请记住，删除现有符号链接并创建新的符号链接的操作不是原子操作（它涉及两个单独的系统调用），因此如果上下文切换发生在中间（即，在移除`/tmp/XYZ`之后），并且目标集UID程序有机会运行它的`fopen(fn, "a+")`语句，它将创建一个以root为所有者的新文件。此后，您的攻击程序将无法再对`/tmp/XYZ`进行更改。**
>
> ​		基本上，使用unlink() 和symlink() 命令接近时，我们的攻击程序中有一个竞态条件。因此，当我们试图利用目标程序中的竞态条件时，目标程序可能会意外地“利用”我们攻击程序中的竞态条件，从而击败我们的攻击。
>
> ​		为了解决这个问题，我们需要使unlink() 和symlink() 原子化。幸运的是，有一个系统调用允许我们实现这一点。更准确地说，它允许我们原子地交换两个符号链接。下面的程序首先创建两个符号链接`/tmp/XYZ`和`/tmp/ABC`，然后使用SYS_renameat2系统调用以原子方式切换它们。这允许我们在不引入任何竞争条件的情况下更改`/tmp/XYZ`指向的内容。应该注意的是，没有用于Ubuntu 16.04上libc库中的SYS_renameat2系统调用。因此我们必须使用syscall() 调用系统调用，而不是像普通函数调用那样调用它。
>
> ```c
> #include <unistd.h>
> #include <sys/syscall.h>
> #include <linux/fs.h>
> 
> int main()
> {
> 	unsigned int flags = RENAME_EXCHANGE;
>     
> 	unlink("/tmp/XYZ"); symlink("/dev/null",  "/tmp/XYZ");
> 	unlink("/tmp/ABC"); symlink("/etc/passwd", "/tmp/ABC");
>     
> 	syscall(SYS_renameat2, 0, "/tmp/XYZ", 0, "/tmp/ABC", flags);
> 	return 0;
> }
> ```
>
> ​		**TASK.** 请使用此新策略修改您的攻击计划，然后重试您的攻击。如果一切都做得正确，你的攻击应该能够成功。





### Task 3: Countermeasure: Applying the Principle of Least Privilege

> ​		本实验室易受攻击程序的根本问题是违反了最小特权原则。程序员知道运行程序的用户可能太强大了，所以他/她引入了access() 	来限制用户的能力。然而，这不是正确的方法。更好的办法是适用最低特权原则；也就是说，如果用户不需要某些特权，则需要禁用该特权。
>
> ​		我们可以使用seteuid系统调用暂时禁用root权限，然后在必要时启用它。请使用此方法修复程序中的漏洞，然后重复攻击。你能成功吗？请报告您的观察结果并提供解释。

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main()
{
	uid_t real_uid = getuid(); // get real user id
	uid_t effective_uid = geteuid(); // get effective user id
    
	char * fn = "/tmp/XYZ";
	char buffer[60];
	FILE *fp;
    
	/* get user input */
	scanf("%50s", buffer );
    
	seteuid (real_uid);
    
	if(!access(fn, W_OK)){
        fp = fopen(fn, "a+");
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
	}else {
        printf("No permission \n");
    }
    
	seteuid (effective_uid);
    
    return 0
}
```

使用seteuid系统调用临时禁用root权限之后，无法再攻击成功。



### Task 4: Countermeasure: Using Ubuntu’s Built-in Scheme

> ​		Ubuntu 10.10及更高版本附带了一个针对竞争条件攻击的内置保护方案。在此任务中，需要使用以下命令重新启用保护：
>
> ```bash
> // On Ubuntu 12.04, use the following command:
> sudo sysctl -w kernel.yama.protected_sticky_symlinks=1
> // On Ubuntu 16.04, use the following command:
> sudo sysctl -w fs.protected_symlinks=1
> ```
>
> ​		在防护装置打开后进行攻击。请描述你的观察结果。请解释以下内容：（1）该保护计划是如何运作的？（2） 这个计划有什么限制?

打开kernel.yama.protected_sticky_symlinks对符号链接的保护后，无法再攻击成功。

- protected_sticky_symlinks参数旨在对符号链接的保护，当它检测到权限所有者和符号链接指向的文件所有者权限不匹配时，则不建立链接，抛出异常。
- 此计划限制了不同用户之前的符号链接访问，也就是说我以普通用户seed创建的符号链接，其他用户无论是以普通用户身份还是root特权都无法访问该符号链接。
  



