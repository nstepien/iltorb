'use strict';

exports.compress = compress;
exports.decompress = decompress;
exports.compressSync = compressSync;
exports.decompressSync = decompressSync;
exports.compressStream = compressStream;
exports.decompressStream = decompressStream;

var encode = require('./build/Release/encode.node');
var decode = require('./build/Release/decode.node');
var Transform = require('stream').Transform;

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

// We need to fill the blockSize for better compression results
function compressStreamChunk(stream, chunk, encoder, status, done) {
  var length = chunk.length;

  if (length > status.remaining) {
    var slicedChunk = chunk.slice(0, status.remaining);
    chunk = chunk.slice(status.remaining);
    status.remaining = status.blockSize;

    encoder.copy(slicedChunk);
    encoder.encode(false, function(err, output) {
      if (err) {
        return done(err);
      }
      if (output) {
        stream.push(output);
      }
      compressStreamChunk(stream, chunk, encoder, status, done);
    });
  } else if (length < status.remaining) {
    status.remaining -= length;
    encoder.copy(chunk);
    done();
  } else { // length === status.remaining
    status.remaining = status.blockSize;
    encoder.copy(chunk);
    encoder.encode(false, function(err, output) {
      if (err) {
        return done(err);
      }
      if (output) {
        stream.push(output);
      }
      done();
    });
  }
}

function compressStream(params) {
  var encoder = new encode.StreamEncode(params || {});
  var blockSize = encoder.getBlockSize();
  var status = {
    blockSize: blockSize,
    remaining: blockSize
  };

  return new Transform({
    transform: function(chunk, encoding, next) {
      compressStreamChunk(this, chunk, encoder, status, next);
    },
    flush: function(done) {
      var that = this;
      encoder.encode(true, function(err, output) {
        if (err) {
          return done(err);
        }
        if (output) {
          that.push(output);
        }
        done();
      });
    }
  });
}

function decompressStream() {
  var decoder = new decode.StreamDecode();

  return new Transform({
    transform: function(chunk, encoding, next) {
      var that = this;
      decoder.transform(chunk, function(err, output) {
        if (err) {
          return next(err);
        }
        if (output) {
          that.push(output);
        }
        next();
      });
    },
    flush: function(done) {
      var that = this;
      decoder.flush(function(err, output) {
        if (err) {
          return done(err);
        }
        if (output) {
          that.push(output);
        }
        done();
      });
    }
  });
}
