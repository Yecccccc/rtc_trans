# rtc_trans
用于将WebRTC捕捉到的视频帧进行解码转换

1. 将RTC端捕捉到的视频帧放入psnr_rtc_1文件夹里

    psnr_rtc_1文件夹包含接受方日志receiver_frame.txt，发送方日志sender_frame.txt，以及帧内容bin文件

2. 安装ffmpeg API

    参考：https://zhuanlan.zhihu.com/p/80895966
    
    ffmpeg存放的位置在/opt/ffmpeg/bin

    修改CMAKELists.txt中的设置，分别是ffmpeg动态库的位置和ffmpeg的头文件位置：

    set(ffmpeg_lib_dir /opt/ffmpeg/lib)
    
    set(ffmpeg_head_dir /opt/ffmpeg/include)

3. cmake. & make & ./demo_yec

    编译demo_yec文件，运行该文件可以将psnr_rtc_1文件夹包含的帧信息转换为webm
    
    tips:
    
    - WebRTC使用的是VP8编码器，所以产生的播放文件最后是webm，webm可以用浏览器或者ffplay播放
    - 由于分辨率会随着网络状态/设备性能变化，所以最后产生的webm文件是多个
    - 一般分辨率切换就会重新编码一个关键帧，所以一个gop一般对应着一个分辨率

4. 使用ffmpeg获取GOP的单帧图片（错误）

    ffmpeg -i vp8_gop2.webm -r 30 -f image2 demo-%3d.jpeg

    得到jpeg数和gop2的总帧数不一致

    原因：ffmpeg对于单个帧的获取是将文件进行播放，然后按照间隔截图，得到jpeg文件

5. 使用libvpx进行vp8的解码

    github地址：https://github.com/webmproject/libvpx

    构建：
    - git clone
    - ./configure
    - make -j8
    
    对webm文件进行转换：
    
    ./vpxdec /home/yec/Desktop/rtc_trans/psnr_rtc_1/vp8_gop2.webm --rawvideo -o demo.yuv
    
    得到demo.yuv文件

    wemb文件就是由yuv视频帧（亮度、色度）进行编码出来的

6. TODO：

    - 将yuv视频文件转换为rgb，将receiver_frame.txt与sender_frame.txt对应起来，根据index找到对应rgb
    - 编写python脚本计算psnr
    - bug:index有错误，再根据frame_size进行双重保证