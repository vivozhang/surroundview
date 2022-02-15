# surroundview
基于opengl es技术进行环视拼接

1、目录结构
(1) data文件夹为图像数据
(2) openglwindow文件夹为360环视拼接代码模块
(3) test文件夹为相机位置权重文件
2、代码分析
函数入口：openglwindow/main.cpp为函数入口
	     其中：initialize() 加载图像、加载相机参数、窗口初始化
		              render() 每次刷新都调用，包括图像数据更新、更新执行
3、