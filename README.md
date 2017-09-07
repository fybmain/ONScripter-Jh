#ONScripter-Jh

>本项目为[ONScripter-Jh][]的一个fork，致力于修复在Linux下编译出现的各种神奇错误。

**本程序在[GPLv2][]协议下发布，使用时请遵守。（详见同目录下的COPYING文件）**

>Onscripter是一个用来解析NScripter脚本的第三方开源Galgame游戏引擎，主要由Ogapee（<ogapee@aqua.dti2.ne.jp>）开发维护。

>[ONScripter-Jh][]修改自[ONScripter][]，旨在在兼容原版[Onscripter][]的前提下，添加中文支持，提高性能，增加功能，发起人为h j（<https://bitbucket.org/jh10001/>）。

##编译
* Linux：
```
	$ make -f Makefile.Linux
	$ onscripter
```

* Windows：
```
$ nmake -f Makefile.Win
$ onscripter.exe
```
* 其余平台编译可参照以上示例和原版[ONScripter][]编译指导无师自通。（逃

##TODO
* 整理这些乱七八糟中用不中看的代码
* 更新至Ogapee发布的最新版本

[GPLv2]: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
[ONScripter]: https://onscripter.osdn.jp/onscripter.html
[ONScripter-Jh]: https://bitbucket.org/jh10001/onscripter-jh

