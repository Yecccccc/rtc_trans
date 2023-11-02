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
#include <memory>
#include <fstream>

using namespace std;

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


int main(int argc, char** argv) {
    string path;
    if (argc > 1) path = argv[1];
    else cout << "input parameters error!";
    // 视频相关信息的初始化
    int videocount = 1;
        // from log to file
    // 文件读取
    string folderpath = "/home/yuqian/pro3/videos/videosets/" + path;
    string infilepath = folderpath + "/out/outlog.txt";
    ifstream infile; 
    infile.open(infilepath); 
    string s;

    // 获取视频信息
    int codec = 2;
    int width = 320;
    int height = 240;
    uint32_t size = 0;
    int64_t timestamp, key_frame;
    getline(infile, s);
    getline(infile, s);
    // int keyframeindex = s.find("keyframe");
    // int idindex = s.find("id");
    // int windex = s.find("w");
    // int hindex = s.find("h");
    // int sizeindex = s.find("size");
    // int receiveindex = s.find("receive");
    // int timestampindex = s.find("time:");
    vector<string> splits = split(s, "\t");
    key_frame = stol(splits[1]);
    size = stoul(splits[2]);
    width = stoi(splits[3]);
    height = stoi(splits[4]);
    timestamp = stol(splits[7]);

    while (s.size() > 1) {
        int count = 0;
        string videopath = folderpath + "/video/" + to_string(videocount) + ".webm";
        ifstream video;
        video.open(videopath);
        string outvideopath = folderpath + "/out/" + to_string(videocount) + ".webm";
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

        // 查找编解码器
        enum AVCodecID video_codec_id = AV_CODEC_ID_NONE;
            switch (codec) {
                case 1:
                    video_codec_id = AV_CODEC_ID_VP8;
                    break;
                case 2:
                    video_codec_id = AV_CODEC_ID_VP9;
                    break;
                case 3:
                    video_codec_id = AV_CODEC_ID_H264;
                    break;
                default:
                    break;
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
        par->width = width;
        par->height = height;


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
        std::shared_ptr<Frame> prev_frame;
        std::shared_ptr<Frame> currframe;
        uint8_t* data = nullptr;

        //将文件内容写为视频帧
        // 101 为GOP组大小，该值可预设，也可从log中获取
        int flag = 1;
        while(key_frame == 0 || flag == 1) {
            flag = 0;
            //读取视频信息
            data = new uint8_t[size];
            video.read((char *)data, size);
            // 写入当前帧
            std::shared_ptr<Frame> tmpptr(new Frame(data, size));
            currframe = tmpptr;
            delete []data;
            currframe->is_key_frame = key_frame;
            currframe->timestamp = timestamp;
            getline(infile, s);
            if (s.size() <= 1) break;
            splits = split(s, "\t");
            key_frame = stol(splits[1]);
            size = stoul(splits[2]);
            width = stoi(splits[3]);
            height = stoi(splits[4]);
            timestamp = stol(splits[7]);//帧处理
            if (!prev_frame) {
                prev_frame = currframe;
                continue;
            }
            prev_frame->duration = currframe->timestamp - prev_frame->timestamp;
            if (prev_frame->duration <= 0) {
                prev_frame->duration = 1;
                currframe->timestamp = prev_frame->timestamp + 1;
                cout << "duration error" << endl;
            }
            frames_.push(prev_frame);
            prev_frame = currframe;
        }
        currframe->duration = 30;
        frames_.push(currframe);
        timestamp_offset = frames_.front()->timestamp;
        // 将帧写入视频文件
        while (!frames_.empty()) {
            std::shared_ptr<Frame> frame = frames_.front();
            frames_.pop();

            AVStream* stream = video_stream;
            AVPacket pkt;

            // 包处理
            av_init_packet(&pkt);
            pkt.data = frame->payload;
            pkt.size = frame->length;
            pkt.dts = (int64_t)((frame->timestamp - timestamp_offset) /
                                (av_q2d(stream->time_base) * 1000));
            //cout << frame->timestamp << " "  << frame->timestamp - timestamp_offset << endl;
            pkt.pts = pkt.dts;
            
            pkt.duration =
                (int64_t)(frame->duration / (av_q2d(stream->time_base) * 1000));
            //cout << frame->duration << "   " << av_q2d(stream->time_base) <<  endl;
            //cout << pkt.duration << endl;
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
        video.close();
        cout << count << endl;
        videocount ++;
    }
    
    infile.close();
}