一、本项目文件由两部分组成：
源代码及VS项目文件夹：cg-final
可执行文件及绘制结果文件夹：package

二、作业结果在"./package/results"下，包含三个原场景以及自定义场景"cornell-box-simple"，所有的场景均在64 samples per pixel的设置下绘制得到。

三、执行程序可以直接在package文件夹下进行，可传入两个自选参数，
分别指示导入"runtime/scene/##argv[1]"下的场景，以及使用argv[2] * argv[2] samples per pixel的设置进行绘制
绘制完成后程序会打印"Hello World!"提示，绘制结果为文件"runtime/output/rgba.png"

示例代码：
\package> .\cg-final.exe veach-mis 3

四、一些内置到源代码的设置：（详见cg-final文件夹下的工程源代码）
1. 本项目参考了http://10.76.1.181/courses/training/mitF04/calendar.html中使用grid对光线追踪的求交进行加速
默认的grid划分参数为（20, 20, 20）
2. 本项目使用Russian Roulette的方式减少递归次数，默认阈值为0.99

五、源代码项目简介
./cg-final.cpp: 实现主体流程，包括场景导入及初始化、grid初始化、分像素点进行路径追踪、最终图像输出
./my_obj_loader.hpp && ./my_scene_loader.hpp: 调用开源库导入obj\mtl\xml文件，并进行预处理
./scene/: 包含场景中各元素的数据结构，scene_paser用于组织场景，其下包含数个三角形面片triangle，
	每个三角形有对应的材质material，有的材质会附加贴图texture；场景中还有相机camera和用于加速的结构grid；
	grid实现需要bounding_box, object3d_vector的基本数据结构支持
./render/: 包含绘制所需的各数据结构，表示光线的ray以及求交结果hit，本项目核心的路径追踪算法在ray_tracer.hpp中实现，
	使用了直接抽样光源、多重重要性采样等方法优化了此路径追踪的实现。


六、项目参考资料：
grid加速：http://10.76.1.181/courses/training/mitF04/calendar.html
obj文件导入：https://github.com/tinyobjloader/tinyobjloader
xml文件导入：http://sourceforge.net/projects/tinyxml/
图像文件导入：http://nothings.org/stb_image.h
图像文件输出：http://github.com/miloyip/svpng