## 文件结构
   bin --二进制文件
   build --中间文件夹路径
   cmake --cmake函数文件夹
   CMakeLists.txt -- cmake的定义文件
   lib -- 库的输出路径
   tests -- 测试代码

## 开发中的一些坑
    1. 在使用了FileAppender的logger中，需要程序结束时析构掉appender里面的fstream(不然内容没写进去)。
    而原先log管理类使用了懒汉单例，直接new的对象(堆上)，
    发现程序退出时并不会析构掉Manager类(在堆上，需要手动析构), fstream并没有close，日志打不进文件。
    解决办法是使用智能指针去管理单例，这样退出时全局智能指针被析构掉，相应的单例资源也被析构掉了。
