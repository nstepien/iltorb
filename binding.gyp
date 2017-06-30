{
  "targets": [
    {
      "target_name": "encode",
      "sources": [
        "brotli/common/dictionary.c",
        "brotli/enc/backward_references.c",
        "brotli/enc/backward_references_hq.c",
        "brotli/enc/bit_cost.c",
        "brotli/enc/block_splitter.c",
        "brotli/enc/brotli_bit_stream.c",
        "brotli/enc/cluster.c",
        "brotli/enc/compress_fragment.c",
        "brotli/enc/compress_fragment_two_pass.c",
        "brotli/enc/dictionary_hash.c",
        "brotli/enc/encode.c",
        "brotli/enc/entropy_encode.c",
        "brotli/enc/histogram.c",
        "brotli/enc/literal_cost.c",
        "brotli/enc/memory.c",
        "brotli/enc/metablock.c",
        "brotli/enc/static_dict.c",
        "brotli/enc/utf8_util.c",
        "src/common/allocator.cc",
        "src/common/stream_coder.cc",
        "src/enc/encode_index.cc",
        "src/enc/stream_encode.cc",
        "src/enc/stream_encode_worker.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "brotli/include"
      ],
      "defines": ["NOMINMAX"],
      "cflags" : [
        "-include ../src/_memcpy.h",
        "-O2"
      ],
      "xcode_settings": {
        "OTHER_CFLAGS" : ["-O2"]
      }
    },
    {
      "target_name": "decode",
      "sources": [
        "brotli/common/dictionary.c",
        "brotli/dec/bit_reader.c",
        "brotli/dec/decode.c",
        "brotli/dec/huffman.c",
        "brotli/dec/state.c",
        "src/common/allocator.cc",
        "src/common/stream_coder.cc",
        "src/dec/decode_index.cc",
        "src/dec/stream_decode.cc",
        "src/dec/stream_decode_worker.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "brotli/include"
      ],
      "cflags" : [
        "-include ../src/_memcpy.h",
        "-O2"
      ],
      "xcode_settings": {
        "OTHER_CFLAGS" : ["-O2"]
      }
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [
        "encode",
        "decode"
      ],
      "copies": [
        {
          "files": [
            "<(PRODUCT_DIR)/encode.node",
            "<(PRODUCT_DIR)/decode.node"
          ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
