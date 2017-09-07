# 补丁
文件linux-make-fix-20170907.patch中包含了本项目基于[ONScripter-Jh](https://bitbucket.org/jh10001/onscripter-jh)最新的[2016-12-01](https://bitbucket.org/jh10001/onscripter-jh/commits/e0eaf62c1cd6)版本所做的修改，内容如下：
1. 修复在linux下编译的各种神奇错误，在Fedora26上编译通过。
2. 合并原版本中的README与README.old

## 使用方法
```
$ patch -p1 <linux-make-fix-20170907.patch
```

