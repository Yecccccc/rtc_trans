ts1 = 1613014572
ts2 = 2466985560
detla = ts1 - ts2
with open("./psnr_rtc_1/receiver_frame.txt", "r") as file:
    content = file.read()
lines = content.split("\n")
gopstart = 0
gopend = 0
timestamp = 0
key_frame = 0
goplist = []
width_list = []
height_list = []
for i in range(len(lines)):
    line = lines[i]
    if line.startswith("timestamp="):
        timestamp = int(line.split("=")[1])
    elif line.startswith("key_frame="):
        key_frame = int(line.split("=")[1])
        if key_frame == 1:
            if gopstart == 0:
                gopstart = timestamp
            else:
                gopend = timestamp
                goplist.append([gopstart - detla, gopend - detla])
                gopstart = timestamp
            # 将i+1的width和height加入 list
            width = int(lines[i + 1].split(" ")[0].split("=")[1])
            height = int(lines[i + 1].split(" ")[1].split("=")[1])
            width_list.append(width)
            height_list.append(height)
            i += 1
file.close()

goplist.append([gopstart, 1618240422 - detla + 1])

with open("./psnr_rtc_1/sender_frame.txt", "r") as file:
    content = file.read()
lines = content.split("\n")
# 记录每个gop的index

with open("./psnr_rtc_1/gop.txt", "w") as f:
    gop_no = 0
    f.write("gop" + str(gop_no + 1) + ":" + "width=" + str(width_list[gop_no]) + ",height=" + str(height_list[gop_no]) + "\n")
    ts = 0
    index = 0
    for line in lines:
        if line.startswith("frame_index="):
            index = int(line.split("=")[1])
        if line.startswith("timestamp="):
            ts = int(line.split("=")[1])
            if ts < goplist[gop_no][1]:
                f.write(str(index) + "\n")
            else:
                gop_no += 1
                f.write("gop" + str(gop_no + 1) + ":" + "width=" + str(width_list[gop_no]) + ",height=" + str(height_list[gop_no]) + "\n")
                f.write(str(index) + "\n")
f.close()
file.close()