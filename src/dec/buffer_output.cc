#include "buffer_output.h"

int BrotliBufferOutputFunction(void* data, const uint8_t* buf, size_t count) {
  std::string *mem_output = (std::string*)data;
  mem_output->append((char*) buf, count);
  return (int)count;
}

BrotliOutput BrotliInitBufferOutput(std::string* mem_output) {
  BrotliOutput output;
  output.cb_ = &BrotliBufferOutputFunction;
  output.data_ = mem_output;
  return output;
}
