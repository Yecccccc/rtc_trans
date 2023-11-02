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

int main() {
    // 视频相关信息的初始化
    int codec = 2;
    int width = 320;
    int height = 240;
    int64_t timestamp_offset = 0;
    const char* path = "/home/lili/Web/webrtc-checkout/recorder/mycode/demo.webm";
    std::queue<std::shared_ptr<Frame>> frames_;
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

    /*
    if (video_codec_id == AV_CODEC_ID_H264 ||
        video_codec_id == AV_CODEC_ID_H265) {  // extradata
        AVCodecParserContext* parser = av_parser_init(video_codec_id);
        if (!parser) {
            cout  << "Recorder::openStreams error, av_parser_init fail";
            return;
        }

        int size =
            parser->parser->split(nullptr, video_key_frame_->payload,
                                    video_key_frame_->length);
        if (size > 0) {
            par->extradata_size = size;
            par->extradata = (uint8_t*)av_malloc(
                par->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            memcpy(par->extradata, video_key_frame_->payload,
                    par->extradata_size);
        } else {
            cout << "Recorder::openStreams error, can't "
                                    "find video extradata";
        }

        av_parser_close(parser);
    }

    if (video_codec_id == AV_CODEC_ID_H265) {
        par->codec_tag = 0x31637668;  // hvc1
    }
    */

    // 写入文件头
    res = avformat_write_header(context_, nullptr);
    if (res < 0) {
        cout<< "Recorder::openStreams error, avformat_write_header fail "
            << to_string(res);
        return 0;
    }


    // from log to file
    // 文件读取
    ifstream infile; 
    infile.open("/home/lili/Web/webrtc-checkout/src/result/log_frame.txt"); 
    ifstream video;
    video.open("/home/lili/Web/webrtc-checkout/src/result/1.webm");
    string s;

    // 解码信息
    std::shared_ptr<Frame> prev_frame;
    uint32_t size = 0;
    int64_t timestamp, key_frame;
    uint8_t* data = nullptr;
    int count = 0;

    //将文件内容写为视频帧
    // 101 为GOP组大小，该值可预设，也可从log中获取
    while(count < 101) {
        count ++;
        // 读取log信息
        getline(infile, s);
        int keyframeindex = s.find("keyframe");
        int sizeindex = s.find("size");
        int timestampindex = s.find("time");
        key_frame = stol(s.substr(keyframeindex + 9, sizeindex - (keyframeindex + 9)));
        size = stoul(s.substr(sizeindex + 5, timestampindex - (sizeindex + 5)));
        timestamp = stol(s.substr(timestampindex + 5, s.size() - (timestampindex + 5)));
        //读取视频信息
        data = new uint8_t[size];
        video.read((char *)data, size);
        
        // 写入当前帧
        std::shared_ptr<Frame>  currframe (new Frame(data, size));
        delete []data;
        currframe->is_key_frame = key_frame;
        currframe->timestamp = timestamp;
        
        //帧处理
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
        cout << frame->timestamp << " "  << frame->timestamp - timestamp_offset << endl;
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
    infile.close();
    video.close();
}