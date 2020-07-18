### 实验报告



#### 实现管道

利用pipe和dup2

#### 实现重定向

检查重定向标识符，将输出重定向到对应文件中。

#### 实现Ctrlc

使用signal函数检查Ctrlc,并调用函数处理。通过输出到stderr保证格式正确



#### 其他

##### 支持echo $PATH

打印环境变量PATH,调用getenv("$PATH")

##### 支持echo ~

打印环境变量HOME，调用getenv("HOME")

##### 支持； 以及&

样例输入为

```shell
sleep 10 ; echo alias &
```



#### 使用 strace 工具追踪系统调用

#### fstat()

定义函数 int fstat(int fildes,struct stat *buf);

函数说明 fstat()用来将参数fildes所指的文件状态，复制到参数buf所指的

结构中(struct stat)。Fstat()与stat()作用完全相同，不同处在

于传入的参数为已打开的文件描述词。详细内容请参考stat()。

返回值 执行成功则返回0，失败返回-1，错误代码存于errno。

链接:https://baike.baidu.com/item/fstat/8324307?fr=aladdin

#### clone()

类似于fork()和vfork()，Linux特有的系统调用clone()也能创建一个新线程。与前两者不同的是，后者在进程创建期间对步骤的控制更为准确。

但是与fork()不同的是，克隆生成的子进程继续运行时不以调用处为起点，转而去调用以参数func所指定的函数，func又称为子函数。调用子函数时的参数由func_arg指定。经过转换，子函数可对改参数的含义自由解读，例如可以作为整型值（int），也可以视为指向结构的指针。

当函数func返回或者是调用exit()（或者_exit()）之后，克隆产生的子进程就会终止。照例，父进程可以通过wait()一类函数来等待克隆子进程。

因为克隆产生的子进程可能共享父进程内存，所以它不能使用父进程的栈。相反，调用者必须分配一块大小适中的内存空间供子进程的栈使用，同时将这块内存的指针置于参数child_stack中。

链接：https://blog.csdn.net/ren18281713749/java/article/details/94769023

#### lseek()

将文件读写指针相对whence移动offset个字节。操作成功时，返回文件指针相对于文件头的位置。当调用成功时则返回目前的读写位置，也就是距离文件开头多少个字节。若有错误则返回-1，errno 会存放错误代码。

链接:https://blog.csdn.net/qq122261257/article/details/6940703

