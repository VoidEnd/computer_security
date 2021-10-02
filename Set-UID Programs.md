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

6. **setuid函数**：setuid()用来重新设置执行目前进程的用户ID。不过，要让此函数有作用，其有效用户ID必须为0(root)。

   seteuid函数：seteuid()用来重新设置执行目前进程的有效用户ID。

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
   - 执行fork()函数后，产生了新的进程，新的进程的PID、PPID与原来原来的进程不同，说明父子进程是两个不同的进程。fork刚执行后的一段时间内，父子进程有着相同的状态（进程空间中的东西都一样，因为fork采用“写时复制”，一开始父子进程共享物理内存空间）。
   - system()函数实际上就是先执行了fork函数，然后新产生的子进程立刻执行了exec函数。system函数会产生新进程，这就意味着新进程的PID、PPID等与原进程不同。system也会产生新的进程空间，而且新的进程空间是为新的程序准备的，所以和原进程的进程空间没有任何关系。 

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

###### 当程序从用户那里接受输入时，我们可以重定向输入设备，这样程序就可以从文件中接受输入。例如，我们可以使用prog < myfile来提供myfile中的数据作为prog程序的输入。现在，如果prog是root用户拥有的Set-UID程序，我们是否可以使用以下方法来获取这个特权程序，以从/etc/shadow文件中读取?

```shell
$ prog < /etc/shadow
```

```bash
如果拥有Set-UID的prog程序没有对参数进行校验，直接输出用户传入的文件路径，输出/etc/shadow
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

基本上，上面的命令将/tmp/mymore转换为Set-UID程序。现在，因为权限被设置为4700，其他用户不能执行该程序。超级用户使用另一个命令(现在显示)只将执行权限授予Alice。我们并不是假设Alice是完全被信任的。如果Alice只能读取其他4人的文件，这是可以的，但如果Alice可以获得任何特权，如写入其他人的文件，这是不可以的。请阅读更多程序的手册，找出爱丽丝可以做什么来获得更多的特权。

```
more指令是可以调用vi的，如果more具有set-uid权限，那么调用vi同样具有set-uid权限，vi又可以执行系统命令，所以Alice可直接利用此方式以set-uid权限执行任意系统命令
```



## 实验

