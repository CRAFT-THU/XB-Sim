# A SystemC Simulator for ReRam-based Neural Network
This project is a simulator for ReRam Crossbar devices. It can be compiled using cmake and running on Windows/Linux.

### Contec List
- <a href='#Install'>Install</a>
- <a href='#Dataset'>Dataset</a>
- <a href='#Run'>Run</a>
- <a href='#Performance'>Performance</a>
- <a href='#Case'>Case study</a>
- <a href='#Next'>Next work</a>

&nbsp;
&nbsp;

## Install
- Install [SystemC Library](http://www.accellera.org/downloads/standards/systemc)
	* For Linux，download the code to local directory and unzip the source code. Step into its directory, `~/systemc-2.3.2` e.g. and run following commands:
	```Shell
	# Init build directory
	mkdir build
	cd build
	# using cmake to compile systemc
	cmake ..
	make
	```
- (Option)Install [Cuda Library](https://developer.nvidia.com/cuda-downloads)
	* Follow the instruction step on cuda website.
- Configuration
	* Download this repo into your own path.
	* For linux，using Cmake to compile the code。This project has already provide CMakeLists.txt, users could change the link libraries according your own needs. Execute following command in shell:
	```Shell
	# Init build directory
	mkdir build
	cd build
	# using cmake to compile 
	# default not using GPU
	cmake .. -DCMAKE_BUILD_TYPE=Release
	# or using GPU
	cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=on 
	make
	# copy the executable file to upper directory and run
	cp simulator-windows ../
	./simulator-windows
	```

&nbsp;

## Dataset
- The dataset used in this project is the Test part in cifar-10 dataset, 10000 pictures in total. And they are already transformed into 3\*1024(32\*32) and put into `input`.
- Labels of Testset is `labels.csv` and put under source code directory.

&nbsp;


## Run
- Parameter Configuration
	* Circuit parameters(`config.h`)

	| DA Reference Voltage | AD Reference Voltage | DA width | AD width | Crossbar length | Crossbar width | Number of Crossbar in each Tile |
	|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
	| DA_V | AD_V | DA_WIDTH | AD_WIDTH | CROSSBAR_L | CROSSBAR_W | CROSSBAR_N |

	* Neural Network parameters(`config.h`)

	| Kernel size | Input data size | Channels of picture | Size of picture | Number of input picture | pooling size |
	|:-:|:-:|:-:|:-:|:-:|:-:|
	| KERNEL_SIZE | INPUT_SIZE(KERNEL_SIZE\*KERNEL_SIZE) | CHANNELS_3/32/48/80/128 | IMAGE_SIZE_32/16/8 | PICTURE_NUM | POOLING_SIZE_1/2/8 |

- Code Generation of Neural Network Structure
	* This project generate codes by pre-defined template module using Python code `cpp_gen.py`. The template code of each module is:

	| `conv_cpp.template` | `conv_buffer_cpp.template` | `linear_cpp.template` | `linear_buffer_cpp.template`|
	|:-:|:-:|:-:|:-:|
	| Convolution layer module template | Buffer module template between convolution layer  | Fully connected layer module template | Buffer module between fully connected layer |

	The network needed structure list stored in `cpp_gen.py` and users can change this file to generate different NN structure

	* The code generation command is `execute_process` in `CMakeLists.txt`, and it is influenced by compile option USE_CUDA. Users don't have to run code generation command separately. The generated code will be put into `generated` directory.

	It will generate the headers of NN structure `stage_conv_*.h`, `conv_buffer_*.h`, `stage_linear_*.h`, `linear_buffer_*.h`. <br>
	And they will be included in `headers.cpp`, using `sc_signal` to connect layers in `main.cpp`.

&nbsp;


## Performance
- Accuracy
	* AD/DA module included：83.5%
	* with noise：82.4%



&nbsp;


## Case
- Using VGG network to classify CIFAR-10 dataset
	* This project generated a VGG network by default, it contains 15 convolution layers and 2 fully connected layers, all of the weights (high dimension convolution kernel) have been transformed into 2-D matrix in `CROSSBAR_L*CROSSBAR_W` for hte computation of ReRam Crossbar.
	* All weight matrices has been transformed before and stored in `weights`. The weight conversion just like the following figure:
	![avatar](WeightConvert.png)

&nbsp;
&nbsp;


## Next

- Support module replication
- Multi Crossbars in each Tile



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

## 安装
- 安装[SystemC库](http://www.accellera.org/downloads/standards/systemc)
	* 对于Windows用户，将systemc代码下载到本地之后，解压缩代码包。进入systemc文件夹，如`E:\systemc-2.3.2\msvc10\SystemC`中，用VS打开`SystemC.vcxproj`。对打开的项目分别用Debug模式和Release模式编译。
	* 对于Linux/Mac用户，将代码下载到本地之后，解压缩代码包。进入systemc文件夹，如`~/systemc-2.3.2`中，执行如下语句: 
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
	(Debug和Release模式) 项目属性→C/C++→常规→附加包含目录→(SystemC库代码所在目录，如`E:\systemc-2.3.2\src`)<br>
	(Debug模式) 项目属性→链接器→常规→附加库目录→(Debug库所在目录，如`E:\systemc-2.3.2\msvc10\SystemC\Debug`)<br>
	(Release模式) 项目属性→链接器→常规→附加库目录→(Release库所在目录，如`E:\systemc-2.3.2\msvc10\SystemC\Release`)<br>
	* 对于Linux/Mac用户，使用Cmake进行代码编译。本项目已提供CMakeLists.txt文件，可根据需要自行修改链接库的路径。编译代码时需执行如下语句：
	```Shell
	# 创建build文件夹
	mkdir build
	cd build
	# 使用cmake对systemc编译
	# 默认不使用GPU进行计算
	cmake .. -DCMAKE_BUILD_TYPE=Release
	# 或者使用GPU计算
	cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=on 
	make
	# 将可执行文件拷贝到外层文件夹并执行
	cp simulator-windows ../
	./simulator-windows
	```

&nbsp;

## 数据集
- 本项目所用测试数据为cifar-10数据集的测试部分，总计10000张图片，已经将输入数据转换为3\*1024(32\*32)，并且放入`input`文件夹下。
- 测试集标签为`labels.csv`文件，放在项目源代码目录下。

&nbsp;


## 运行
- 参数配置
	* 电路相关参数(`config.h`)

	| DA参考电压 | AD参考电压 | DA宽度 | AD宽度 | Crossbar长度 | Crossbar宽度 | 每个Tile中Crossbar个数 |
	|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
	| DA_V | AD_V | DA_WIDTH | AD_WIDTH | CROSSBAR_L | CROSSBAR_W | CROSSBAR_N |

	* 网络模型相关参数(`config.h`)

	| 卷积核大小 | 输入数据大小 | 图像通道数 | 图像尺寸 | 输入图片数目 | 池化大小 |
	|:-:|:-:|:-:|:-:|:-:|:-:|
	| KERNEL_SIZE | INPUT_SIZE(KERNEL_SIZE\*KERNEL_SIZE) | CHANNELS_3/32/48/80/128 | IMAGE_SIZE_32/16/8 | PICTURE_NUM | POOLING_SIZE_1/2/8 |

- 网络结构代码生成
	* 本项目使用Python代码`cpp_gen.py`，根据预先写好的模块模板来生成代码。各模块的模板文件为如下：

	| `conv_cpp.template` | `conv_buffer_cpp.template` | `linear_cpp.template` | `linear_buffer_cpp.template`|
	|:-:|:-:|:-:|:-:|
	| 卷积层模块模板 | 卷积层间Buffer模块模板 | 全连接层模块模板 | 全连接层间Buffer模块模板 |

	在`cpp_gen.py`文件中，存放了所需网络的结构列表，可通过修改该文件来生成不同的网络结构。

	* 代码生成语句为`CMakeLists.txt`文件中的`execute_process`指令，受编译选项USE_CUDA影响，不需要单独执行代码生成指令。生成的代码将放入`generated`文件夹下。

	可以生成所需的网络结构头文件`stage_conv_*.h`, `conv_buffer_*.h`, `stage_linear_*.h`, `linear_buffer_*.h`. <br>
	在`headers.cpp`文件中包含以上生成的头文件，并且根据各层之间的连接关系，在`main.cpp`文件中通过`sc_signal`进行串联。

&nbsp;


## 性能
- 准确率
	* 包括AD/DA模块：83.5%
	* 加入噪音：82.4%



&nbsp;


## 样例
- 使用VGG-16网络对CIFAR-10数据进行分类
	* 本项目默认生成了一个VGG-16网络，包括15个卷积层和2个全连接层，并且所有的权重（高维卷积核）全部转换为`CROSSBAR_L*CROSSBAR_W`的二维矩阵用于ReRam Crossbar计算。
	* 所有权重矩阵已经预先转换完成，并且存放在`weights`文件夹中。权重矩阵的转换方式如下图所示：
	![avatar](WeightConvert.png)

&nbsp;
&nbsp;


## 下一步工作

- 支持模块复用
- 每个Tile中多个Crossbar的完善
