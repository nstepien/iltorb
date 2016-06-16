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
var util = require('util');

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

function TransformStreamEncode(params) {
  Transform.call(this);

  this.encoder = new encode.StreamEncode(params || {});
  var blockSize = this.encoder.getBlockSize();
  this.status = {
    blockSize: blockSize,
    remaining: blockSize
  };
}
util.inherits(TransformStreamEncode, Transform);

TransformStreamEncode.prototype._transform = function(chunk, encoding, next) {
  compressStreamChunk(this, chunk, this.encoder, this.status, next);
};

TransformStreamEncode.prototype._flush = function(done) {
  var that = this;
  this.encoder.encode(true, function(err, output) {
    if (err) {
      return done(err);
    }
    if (output) {
      that.push(output);
    }
    done();
  }, true);
};

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
    }, true);
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
    }, true);
  }
}

function compressStream(params) {
  return new TransformStreamEncode(params);
}

function TransformStreamDecode() {
  Transform.call(this);

  this.decoder = new decode.StreamDecode();
}
util.inherits(TransformStreamDecode, Transform);

TransformStreamDecode.prototype._transform = function(chunk, encoding, next) {
  var that = this;
  this.decoder.transform(chunk, function(err, output) {
    if (err) {
      return next(err);
    }
    if (output) {
      that.push(output);
    }
    next();
  }, true);
};

TransformStreamDecode.prototype._flush = function(done) {
  var that = this;
  this.decoder.flush(function(err, output) {
    if (err) {
      return done(err);
    }
    if (output) {
      that.push(output);
    }
    done();
  }, true);
};

function decompressStream() {
  return new TransformStreamDecode();
}
