{
  "targets": [
    {
      "target_name": "iltorb",
      "sources": [
        "brotli/c/common/dictionary.c",
        "brotli/c/common/transform.c",

        "brotli/c/dec/bit_reader.c",
        "brotli/c/dec/decode.c",
        "brotli/c/dec/huffman.c",
        "brotli/c/dec/state.c",

        "brotli/c/enc/backward_references.c",
        "brotli/c/enc/backward_references_hq.c",
        "brotli/c/enc/bit_cost.c",
        "brotli/c/enc/block_splitter.c",
        "brotli/c/enc/brotli_bit_stream.c",
        "brotli/c/enc/cluster.c",
        "brotli/c/enc/compress_fragment.c",
        "brotli/c/enc/compress_fragment_two_pass.c",
        "brotli/c/enc/dictionary_hash.c",
        "brotli/c/enc/encode.c",
        "brotli/c/enc/encoder_dict.c",
        "brotli/c/enc/entropy_encode.c",
        "brotli/c/enc/histogram.c",
        "brotli/c/enc/literal_cost.c",
        "brotli/c/enc/memory.c",
        "brotli/c/enc/metablock.c",
        "brotli/c/enc/static_dict.c",
        "brotli/c/enc/utf8_util.c",

        "src/common/allocator.cc",
        "src/common/stream_coder.cc",

        "src/dec/stream_decode.cc",
        "src/dec/stream_decode_tasks.cc",

        "src/enc/stream_encode.cc",
        "src/enc/stream_encode_tasks.cc",

        "src/iltorb.cc"
      ],
      "include_dirs": [
        "brotli/c/include"
      ],
      "defines": ["NOMINMAX"],
      "cflags" : [
        "-O2"
      ],
      "xcode_settings": {
        "OTHER_CFLAGS" : ["-O2"]
      },
      "configurations": {
        "Debug": {
          "conditions": [
            ['OS=="android"', {
              "cflags": ["-fPIC"],
              "ldflags!": ["-pie"]
            }]
          ]
        },
        "Release": {
          "conditions": [
            ['OS=="android"', {
              "cflags": ["-fPIC"],
              "ldflags!": ["-pie"]
            }]
          ]
        }
      }
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [
        "iltorb"
      ],
      "copies": [
        {
          "files": [
            "<(PRODUCT_DIR)/iltorb.node"
          ],
          "destination": "build/bindings"
        }
      ]
    }
  ]
}
