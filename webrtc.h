#include <cstdint>

using namespace std;

class Frame {
public:
  Frame(const uint8_t* payload, uint32_t length);
  ~Frame();
  uint8_t* payload;
  uint32_t length;
  int64_t timestamp;
  int64_t duration;
  int64_t is_key_frame;
};

Frame::Frame(const uint8_t* payload, uint32_t length) {
    this->payload = new uint8_t[length];
    memcpy(this->payload, payload, length);
    this->length = length;
    this->timestamp = 0;
    this->duration = 0;
    this->is_key_frame = 0;
}

Frame::~Frame(){
  delete []payload;
}

class TXT_LOG {
public:
  int64_t timestamp;
  int frame_size;
  bool key_frame;
  int encoded_width;
  int encoded_heigt;
  TXT_LOG() {
    this->timestamp = 0;
    this->frame_size = 0;
    this->key_frame = 0;
    this->encoded_width = 0;
    this->encoded_heigt = 0;
  }
  void clear() {
    this->timestamp = 0;
    this->frame_size = 0;
    this->key_frame = 0;
    this->encoded_width = 0;
    this->encoded_heigt = 0;
  }
  TXT_LOG(int timestamp, int frame_size, bool key_frame, int encoded_width, int encoded_height) 
  {
    this->timestamp = timestamp;
    this->frame_size = frame_size;
    this->key_frame = key_frame;
    this->encoded_width = encoded_width;
    this->encoded_heigt = encoded_height;
  }
};