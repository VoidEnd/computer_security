# Attacks Through Environment Variables

## 知识点

1. **export**：设置或显示环境变量。

   在shell中执行程序时，shell会提供一组环境变量。 export可新增，修改或删除环境变量，供后续执行的程序使用。export的效力仅及于该此登陆操作。

   ```bash
   export -p //列出当前的环境变量值
   ```

   unset：用于删除变量或函数。

2. 编译过程：

   - 预处理：在预处理阶段，编译器主要作加载头文件、宏替换、条件编译的作用。一般处理带“#”的语句。`gcc -E main.c > main.i`
   - 编译：编译器主要作语法检查和词法分析。在确认所有指令都符合语法规则之后，将其翻译成等价的中间代码或者是汇编代码。`gcc -S main.i -o main.s`
   - 汇编：汇编阶段是把编译阶段生成的”.s”文件转成二进制目标代码。`gcc -c main.s -o main.o`
   - 链接：链接就是将目标文件、启动代码、库文件链接成可执行文件的过程，这个文件可被加载或拷贝到存储器执行。`gcc main.o -o main.exe`

   ![img](https://img-blog.csdnimg.cn/img_convert/74350a7799c12749ebd5d4489411786c.png)

   

3. **GCC参数**

   | -c   | 只激活预处理,编译,和汇编,只把程序做成obj文件      | `gcc -c hello.c` ，.o的obj文件     |
   | ---- | ------------------------------------------------- | ---------------------------------- |
   | -S   | 只激活预处理和编译，把文件编译成为汇编代码。      | `gcc -S hello.c`，生成.s的汇编代码 |
   | -o   | 制定目标名称,缺省的时候,gcc 编译出来的文件是a.out | `gcc -o hello.asm -S hello.c`      |

4. **静态库**（.a、.lib）和**动态库**（.so、.dll）

   **静态链接**：在链接阶段，会将汇编生成的目标文件.o与引用到的库一起链接打包到可执行文件中

   **动态链接**：在程序编译时并不会被连接到目标代码中，而是在程序运行是才被载入

5. `gcc -shared -o libmylib.so.1.0.1 sleep.o`

   创建共享库

6. echo：用于字符串的输出，覆盖文件内容，追加文件内容。

7. C 库函数 `int sprintf(char *str, const char *format, ...)` 发送格式化输出到 **str** 所指向的字符串。

8. C 库函数 `char *getenv(const char *name)` 搜索 name 所指向的环境字符串，并返回相关的值给字符串。

9. **pwd**：print work directory

10. **帮助命令man**：用于打开帮助文档

11. **$ 获取变量值**

12. 一般情况下，**动态库加载的加载顺序**为LD_PRELOAD>LD_LIBRARY_PATH>/etc/ld.so.cache>/lib>/usr/lib。

13. 提示:子进程可能不会继承LD_*环境变量

## 习题 

## 实验