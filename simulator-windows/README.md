# 面向Crossbar的SystemC模拟器
本项目是面向Crossbar器件的模拟器，支持Visual Studio和cmake编译，可运行在Windows和Linux(Mac)下。


### 内容列表
- <a href='#安装'>安装</a>
- <a href='#数据集'>数据集</a>
- <a href='#运行'>运行</a>
- <a href='#性能'>性能</a>
- <a href='#样例'>样例</a>
- <a href='#下一步工作'>下一步工作</a>

&nbsp;
&nbsp;
&nbsp;
&nbsp;

## 安装
- 安装[SystemC库](http://www.accellera.org/downloads/standards/systemc)
	* 对于Windows用户，将systemc代码下载到本地之后，解压缩代码包。进入systemc文件夹，如 E:\systemc-2.3.2\msvc10\SystemC 中，用VS打开SystemC.vcxproj。对打开的项目分别用Debug模式和Release模式编译。
	* 对于Linux/Mac用户，将代码下载到本地之后，解压缩代码包。进入systemc文件夹，如~/systemc-2.3.2 中，执行如下语句: 
	```Shell
	# 创建build文件夹
	mkdir build
	cd build
	# 使用cmake对systemc编译
	cmake ..
	make
	```
- (可选)安装[Cuda库](https://developer.nvidia.com/cuda-downloads)
	* 可参照cuda官网的的安装教程进行安装。
- 配置工程文件
	* 对于Windows用户，使用Visual Studio进行开发，可根据本项目中提供的Visual Studio的配置文件进行配置。注意，如下配置须自行修改：<br>
	(Debug和Release模式) 项目属性→C/C++→常规→附加包含目录→(SystemC库代码所在目录，如E:\systemc-2.3.2\src)<br>
	(Debug模式) 项目属性→链接器→常规→附加库目录→(Debug库所在目录，如E:\systemc-2.3.2\msvc10\SystemC\Debug)<br>
	(Release模式) 项目属性→链接器→常规→附加库目录→(Release库所在目录，如E:\systemc-2.3.2\msvc10\SystemC\Release)<br>
	* 对于Linux/Mac用户，使用Cmake进行代码编译。本项目已提供CMakeLists.txt文件，可根据需要自行修改链接库的路径。编译代码时需执行如下语句：
	```Shell
	# 创建build文件夹
	mkdir build
	cd build
	# 使用cmake对systemc编译
	cmake ..
	make
	# 将可执行文件拷贝到外层文件夹并执行
	cp simulator-windows ../
	./simulator-windows
	```

&nbsp;
&nbsp;

## 数据集
- 本项目所用测试数据为cifar-10数据集，已经将输入数据转换为3\*1024(32\*32)，并且放入input文件夹下。

&nbsp;
&nbsp;


## 运行

&nbsp;
&nbsp;


## 性能

&nbsp;
&nbsp;


## 样例

&nbsp;
&nbsp;


## 下一步工作
