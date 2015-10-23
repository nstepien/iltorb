#ifndef BUFFER_OUTPUT_H
#define BUFFER_OUTPUT_H

#include <nan.h>
#include "../../brotli/dec/decode.h"

int BrotliBufferOutputFunction(void* data, const uint8_t* buf, size_t count);

BrotliOutput BrotliInitBufferOutput(std::string* mem_output);

#endif
