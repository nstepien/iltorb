'use strict';

exports.compress = compress;
exports.decompress = decompress;
exports.compressSync = compressSync;
exports.decompressSync = decompressSync;
exports.compressStream = compressStream;
exports.decompressStream = decompressStream;

const encode = require('./build/Release/encode.node');
const decode = require('./build/Release/decode.node');
const Transform = require('stream').Transform;

function compress(input, params, cb) {
  if (arguments.length === 2) {
    cb = params;
    params = {};
  }
  if (!Buffer.isBuffer(input)) {
    process.nextTick(cb, new Error('Brotli input is not a buffer.'));
    return;
  }
  if (typeof cb !== 'function') {
    process.nextTick(cb, new Error('Second argument is not a function.'));
    return;
  }
  encode.compressAsync(input, params, cb);
}

function decompress(input, cb) {
  if (!Buffer.isBuffer(input)) {
    process.nextTick(cb, new Error('Brotli input is not a buffer.'));
    return;
  }
  if (typeof cb !== 'function') {
    process.nextTick(cb, new Error('Second argument is not a function.'));
    return;
  }
  decode.decompressAsync(input, cb);
}

function compressSync(input, params) {
  if (!Buffer.isBuffer(input)) {
    throw new Error('Brotli input is not a buffer.');
  }
  params = params || {};
  return encode.compressSync(input, params);
}

function decompressSync(input) {
  if (!Buffer.isBuffer(input)) {
    throw new Error('Brotli input is not a buffer.');
  }
  return decode.decompressSync(input);
}

function handleStream(handler, params) {
  let buffer;
  return new Transform({
    transform: function(chunk, encoding, next) {
      if (buffer) {
        buffer = Buffer.concat([buffer, chunk], buffer.length + chunk.length);
      } else {
        buffer = chunk;
      }
      next();
    },
    flush: function(done) {
      let args = [buffer, (err, output) => {
        if (err) {
          return done(err);
        }
        this.push(output);
        done();
      }];
      if (params) {
        args.splice(1, 0, params);
      }
      handler.apply(null, args);
    }
  });
}

function compressStream(params) {
  params = params || {};
  return handleStream(encode.compressAsync, params);
}

function decompressStream() {
  return handleStream(decode.decompressAsync);
}
