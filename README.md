# windows_service_daemon
a windows service which managing other programs   一个守护进程，以windows服务形式存在


基于windows开发，使用VS2015，其他版本可能需要做一下转换。

主要功能：
1.以服务形式存活，并且负责检测、拉起、传参目标程序.
2.根据一些情形，灵活的对目标程序做一些基本控制。
3.使用远端网页进行命令控制，服务拉取网页，做内容解析。

内容示例：
第一个数值为本机id，用来查询对应行
第一个中括号的1表示一天的第几个小时工作，0-23
第二个中括号为启动命令，程序路径和参数
zzxcqwe012[111111111111111111111111][abc.exe -p]
111111111111111111111111
111111111000000000000111



注意事项：
libcurl.dll有一些要求，要求支持ssl，也就是能访问https协议，Release下放了一个libcurl.7z，是随便从系统其他程序复制的，不确定版本，但是能凑合工作。


VaultScv是主工程，其他的是一些周边实验和demo。


最后一个版本采用配置文件设置id，避免一个机器一次编译的麻烦。


TODO：
两个要迭代的点，1是命令地址可以多加一个，防止一个失效。2是本机id使用了配置文件，这个当然已经开发了，为了便捷性，这个文件必须是txt，不然手动编辑的时候权限出问题。
3闲的没事的时候，在不改变健壮性且充分测试的情况下，精简代码（勉强工作真的战战兢兢，一改坏了查一晚）。
4.增加健壮性检查


5.关于sleep时长、快速检测taskmgr与低资源消耗，看情况优化，比如更高频的检测，但不走下边循环？如果不启动，下边那么多行其实也没什么资源消耗，应该一样的
























