from traceback import FrameSummary
import cv2
import numpy as np
# 初始配置
gop_num = 2
labname = "psnr_rtc_2"

# 定义函数计算 PSNR
def calculate_psnr(img1, img2):
    mse = np.mean((img1 - img2) ** 2)
    if mse == 0:
        return float('inf')
    max_pixel = 255.0
    psnr = 10 * np.log10((max_pixel ** 2) / mse)
    return psnr

file_dst = "./" + labname + "/gop" + str(gop_num) + ".rgb"
file_src = "/home/yec/Desktop/video/bbb_gbr.rgb"
index_file = "./" + labname + "/gop.txt"

index_list = []
width = 0
height = 0
with open(index_file) as file:
    contents = file.read()
lines = contents.split("\n")
falg = 0
for line in lines:
    if line.startswith("gop" + str(gop_num)):
        width = int(line.split("=")[1].split(",")[0])
        height = int(line.split("height=")[1])
        falg = 1
    elif line.startswith("gop" + str(gop_num + 1)):
        falg = 0
        break
    elif falg == 1:
        index_list.append(int(line))
file.close()

src_length = 1920 * 1080 * 3
dst_length = width * height * 3
f_src = open(file_src, 'rb')
f_dst = open(file_dst, 'rb')
f_psnr = open("./" + labname + "/gop" + str(gop_num) + "_psnr.txt", 'w')
total_psnr = 0
for i in range(len(index_list)):
    offset_src = 1920 * 1080 * 3 * (index_list[i] % 900)
    f_src.seek(offset_src, 0)
    frame_src = np.fromfile(f_src, dtype=np.uint8, count=src_length)
    frame_src = frame_src.astype(np.float32)
    frame_src = frame_src.reshape(1080, 1920, 3)
    # webrtc输入端是bgr，导致输入端r和b颠倒，所以添加下面一行
    # 这里是接收方转换颜色空间，下面一行用于bgr转换为rgb
    frame_src = frame_src[:, :, ::-1]
    # 测试，观察帧是否对应的上
    if i == 300:
        cv2.imwrite('./pic/src' + str(i) + ".jpeg", frame_src)
    offset_dst = width * height * 3 * i
    f_dst.seek(offset_dst, 0)
    frame_dst = np.fromfile(f_dst, dtype=np.uint8, count=dst_length)
    frame_dst = frame_dst.astype(np.float32)
    frame_dst = frame_dst.reshape(height, width, 3)
    # webrtc输入端是rgb，导致播放端r和b颠倒，所以添加下面一行
    # 这里是接收方转换颜色空间，下面一行用于bgr转换为rgb
    # frame_dst = frame_dst[:, :, ::-1]
    # 测试，观察帧是否对应的上
    if i == 300:
        cv2.imwrite('./pic/dst' + str(i) + ".jpeg", frame_dst)
    # 插值
    frame_dst = cv2.resize(frame_dst, (frame_src.shape[1], frame_src.shape[0]), interpolation=cv2.INTER_LINEAR)

    mse = np.mean((frame_src - frame_dst) ** 2)

    if mse < 1e-5:
        psnr = float('100.00')
    else:
        max_pixel_value = 255  # 假设像素值范围为0到255
        psnr = 10 * np.log10((max_pixel_value ** 2) / mse)
    total_psnr += psnr
    f_psnr.write("index=" + str(index_list[i]) + ",psnr=" + str(psnr) + "dB\n")
print("frame number is " + str(len(index_list)))
print("gop" + str(gop_num) + " average psnr=" + str(total_psnr/len(index_list)))

f_src.close()
f_dst.close()
f_psnr.close()