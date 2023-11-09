// 记录webrtc收到的视频块
#include <stdio.h>
extern"C"
{
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
    #include "libavfilter/avfilter.h"
}
#include <iostream>
#include "webrtc.h"
#include <queue>
#include <vector>
#include <memory>
#include <fstream>

using namespace std;
// 编解码器设置，指定编解码方式为H264
AVCodecID video_codec_id = AV_CODEC_ID_VP8;

// 存放raw data
string bin_path = "./psnr_rtc_2/frame";
// 记录接收方收到的帧信息
string receiver_txt_path = "./psnr_rtc_2/receiver_frame.txt";
// 输出路径
string outpath = "./psnr_rtc_2/";
// gop_num
int gop_num = 1;
int main() {
    // 视频相关信息的初始化
    int videocount = 1;
    ifstream infile; 
    infile.open(receiver_txt_path);
    if (!infile.is_open()) {
        cerr << "Failed to open log file" << endl;
        return 1;
    }
    string line;
    vector <TXT_LOG> log_vec;
    TXT_LOG temp;
    // 读取txt文件得到帧的所有信息
    while (getline(infile, line)) {
        if (line == "") {
            log_vec.push_back(temp);
            temp.clear();
        }
        else if (line.find("timestamp=") != string::npos) {
            temp.timestamp = stoll(line.substr(line.find("=") + 1));
        }
        else if (line.find("frame_size=") != std::string::npos) {
            temp.frame_size = stoi(line.substr(line.find("=") + 1));
        }
        else if (line.find("key_frame=") != std::string::npos) {
            temp.key_frame = stoi(line.substr(line.find("=") + 1));
        } 
        else if (line.find("encoded_width=") != std::string::npos) {
            // stoi直接截断了后面
            temp.encoded_width = stoi(line.substr(line.find("encoded_width=") + 14));
            temp.encoded_heigt = stoi(line.substr(line.find("encoded_height=") + 15));
        }
    }
    infile.close();

    int index = 0;
    while (index < log_vec.size()) {
        int count = 0;

        string outvideopath = outpath + "vp8_gop" + to_string(videocount) + ".webm";
        char path [100];
        strcpy(path, outvideopath.c_str()); 
        // 解码帧相关信息        
        // ffmpeg相关信息初始化
        AVFormatContext* context_ = nullptr;
        AVCodecParameters* par = nullptr;
        int res;

        // 定义AVformat
        avformat_alloc_output_context2(&context_, nullptr, nullptr, path);
        if (!context_) {
            cout << "Recorder::Start error, alloc context fail";
            return -11;
        }
        
        //打开输出文件
        res = avio_open(&context_->pb, context_->url, AVIO_FLAG_WRITE);
        if (res < 0) {
            cout << "Recorder::Start error, open fail "
                                << to_string(res);
            avformat_free_context(context_);
            context_ = nullptr;
            return -12;
        }

        // 创建视频流
        AVStream* video_stream = avformat_new_stream(context_, nullptr);
        if (!video_stream) {
            cout << "Recorder::openStreams error, open video stream fail";
            return 0;
        }

        // 管理编解码器参数
        par = video_stream->codecpar;
        par->codec_type = AVMEDIA_TYPE_VIDEO;
        par->codec_id = video_codec_id;
        par->width = log_vec[index].encoded_width;
        par->height = log_vec[index].encoded_heigt;


        // 写入文件头
        res = avformat_write_header(context_, nullptr);
        if (res < 0) {
            cout<< "Recorder::openStreams error, avformat_write_header fail "
                << to_string(res);
            return 0;
        }
        // 解码信息
        int64_t timestamp_offset = 0;
        std::queue<std::shared_ptr<Frame>> frames_;
        int falg = 1;
        int width = log_vec[index].encoded_width;
        int height = log_vec[index].encoded_heigt;
        while (index < log_vec.size() && (falg == 1 || log_vec[index].key_frame == false)) {
            uint8_t* data = nullptr;
            int64_t timestamp =log_vec[index].timestamp;
            int size = log_vec[index].frame_size;
            bool key_frame = log_vec[index].key_frame;
            data = new uint8_t[size];
            ifstream video;
            video.open(bin_path + "/" + to_string(timestamp) + ".bin");
            video.read((char *)data, size);
            video.close();
            shared_ptr<Frame> tmpptr = make_shared<Frame> (data, size);
            tmpptr->is_key_frame = key_frame;
            tmpptr->timestamp = timestamp;
            if (falg == 0) {
                // 第一帧
                tmpptr->duration = log_vec[index + 1].timestamp - tmpptr->timestamp;
            }
            else {
                // 最后一帧
                if (index == log_vec.size() - 1)
                    tmpptr->duration = 33;
                else {
                    // 由于timestamp 的单位问题，除以100
                    tmpptr->duration = log_vec[index + 1].timestamp - tmpptr->timestamp;
                    tmpptr->duration /= 100;
                }
            }
            if (tmpptr->duration < 0) {
                cout << "duration error, the duration is " << tmpptr->duration 
                << " the timestamp is "  << tmpptr->timestamp << " the index is " << index << endl;
                tmpptr->duration = 1;
            }
            falg = 0;
            frames_.push(tmpptr);
            index++;
        }

        timestamp_offset = frames_.front()->timestamp;
        // 将帧写入视频文件
        while (!frames_.empty()) {
            shared_ptr<Frame> frame = frames_.front();
            frames_.pop();

            AVStream* stream = video_stream;
            AVPacket pkt;

            // 包处理
            av_init_packet(&pkt);
            pkt.data = frame->payload;
            pkt.size = frame->length;
            // 由于timestamp单位的问题，除以100
            pkt.dts = (int64_t)((frame->timestamp - timestamp_offset)/ 100 /
                                 (av_q2d(stream->time_base) * 1000));
            pkt.pts = pkt.dts;
            
            pkt.duration = (int64_t)(frame->duration / (av_q2d(stream->time_base) * 1000));
            pkt.stream_index = stream->index;
            if (frame->is_key_frame) {
                pkt.flags |= AV_PKT_FLAG_KEY;
            }
            // 写入帧
            res = av_interleaved_write_frame(context_, &pkt);
            count++;
            if (res < 0) {
                cout << "Recorder::drainFrames error, av_interleaved_write_frame fail "
                                << to_string(res);
            }
        }
        // 写入文件尾部
        if (context_) {
            if (video_stream) {
                av_write_trailer(context_);
            }
            avio_close(context_->pb);
            avformat_free_context(context_);
            context_ = nullptr;
        }
        cout << count << endl;
        videocount++;
    }

    return 0;
}