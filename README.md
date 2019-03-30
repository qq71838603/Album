# album
6818开发板实现电子相册以及音乐播放器
项目实施步骤：
      1.先搞定递归读取目录的代码 ---》至少能够完整的将整个目录已经嵌套目录里面的内容全部读取
      2.搞定判断bmp和mp3
      3.搞定触摸屏


触摸屏操作：
      思路：打开触摸屏的驱动(/dev/input/event0  新的6818开发板),--》读取触摸屏的坐标--》关闭触摸屏
                            (/dev/event0  旧的210开发板) 
      新知识点：linux中触摸屏的驱动是采用输入子系统模型来写的
                输入子系统模型：linux将所有输入类型设备(键盘，鼠标，触摸屏)的驱动和应用程序抽象为统一的模型(方便程序员开发)
                /usr/include/linux/input.h定义了输入子系统模型有关的宏定义
                其中定义一个结构体，存放相关宏定义的
                   struct input_event
                   {
                        type;  //事件类型  EV_KEY表示键盘  EV_ABS表示触摸屏  EV_REL表示鼠标
                        code;  //按键，x，y坐标，鼠标左右键
                        value; //按键值，坐标值
                    
                   }

总结目前常用的操作字符串的函数：
    strcpy()  //拷贝字符串，字符串不能直接赋值的
        char *strcpy(char *dest, const char *src);
        char *strncpy(char *dest, const char *src, size_t n); //n表示拷贝前面n个字节

    strstr()  //寻找子串
    strcmp()  //字符串的比较  if("hello"=="HELLO")错误  >  <  !=  ==(不能使用这些符号)
        int strcmp(const char *s1, const char *s2);
                  返回值： 0  相等
                          >0  s1大于s2
                          <0  s1小于s2
        int strncmp(const char *s1, const char *s2, size_t n);//n表示比较前面n个字节
   strtok()
   strcat()
   

项目经验：
    1.编程的时候标志位很普遍
    2.参数传递，通过返回值，通过形参，通过全局变量



提升功能分享
================
     1.音乐，bmp的删除，添加



如何整体上把握，理顺项目的思路
================
    参见笔记中代码提示


主界面和子菜单之间切换

===============
   用switch()  case改造代码 //仔细体会一下代码


音乐相关的功能
===============


linux定义了62个信号，通过kill -l命令查看到所有的信号名以及编号
     1.音乐的播放
          在secure CRT中输入 madplay  音乐的路径名  //输入命令播放
          #include <stdlib.h>
            int system(const char *command);
                    参数：command --》你要执行的完整的命令

            system("madplay /IOT/mp3/test.mp3 &"); //&在linux中表示后台播放
                      必须后台播放，才能暂停，继续，退出
            system("madplay /IOT/mp3/test.mp3"); //前台播放

            system("cp /1.txt  /haha");
      音乐的暂停
            system("killall  -STOP madplay"); //给madplay发送暂停信号
            system("killall  -19 madplay");
      音乐的继续
            system("killall  -CONT madplay"); //给madplay发送继续信号
      音乐的退出
            system("killall  -KILL madplay"); //给madplay发送退出信号
      下一首：思路关闭正在播放的音乐，然后播放下一首    
    


    2.音量调节
            madplay  --help能够查询到所有的命令
            其中
                  madplay  test.mp3  --attenuate=-10    //值的范围 -175到+18
                  

