'use strict';

exports.compress = compress;
exports.decompress = decompress;
exports.compressSync = compressSync;
exports.decompressSync = decompressSync;
exports.compressStream = compressStream;
exports.decompressStream = decompressStream;

const encode = require('./build/bindings/encode.node');
const decode = require('./build/bindings/decode.node');
const Transform = require('stream').Transform;

class TransformStreamEncode extends Transform {
  constructor(params, sync) {
    super(params);
    this.sync = sync || false;
    this.encoder = new encode.StreamEncode(params || {});
    const blockSize = this.encoder.getBlockSize();
    this.status = {
      blockSize,
      remaining: blockSize
    };
  }

  _transform(chunk, encoding, next) {
    compressStreamChunk(this, chunk, this.encoder, this.status, this.sync, next);
  }

  _flush(done) {
    this.encoder.encode(true, (err, output) => {
      if (err) {
        return done(err);
      }
      if (output) {
        for (let i = 0; i < output.length; i++) {
          this.push(output[i]);
        }
      }
      done();
    }, !this.sync);
  }
}

class TransformStreamDecode extends Transform {
  constructor(params, sync) {
    super(params);
    this.sync = sync || false;
    this.decoder = new decode.StreamDecode(params || {});
  }

  _transform(chunk, encoding, next) {
    this.decoder.transform(chunk, (err, output) => {
      if (err) {
        return next(err);
      }
      if (output) {
        for (let i = 0; i < output.length; i++) {
          this.push(output[i]);
        }
      }
      next();
    }, !this.sync);
  }

  _flush(done) {
    this.decoder.flush((err, output) => {
      if (err) {
        return done(err);
      }
      if (output) {
        for (let i = 0; i < output.length; i++) {
          this.push(output[i]);
        }
      }
      done();
    }, !this.sync);
  }
}

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
  const stream = new TransformStreamEncode(params);
  const chunks = [];
  let length = 0;
  stream.on('error', cb);
  stream.on('data', function(c) {
    chunks.push(c);
    length += c.length;
  });
  stream.on('end', function() {
    cb(null, Buffer.concat(chunks, length));
  });
  stream.end(input);
}

function decompress(input, params, cb) {
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
  const stream = new TransformStreamDecode(params);
  const chunks = [];
  let length = 0;
  stream.on('error', cb);
  stream.on('data', function(c) {
    chunks.push(c);
    length += c.length;
  });
  stream.on('end', function() {
    cb(null, Buffer.concat(chunks, length));
  });
  stream.end(input);
}

function compressSync(input, params) {
  if (!Buffer.isBuffer(input)) {
    throw new Error('Brotli input is not a buffer.');
  }
  const stream = new TransformStreamEncode(params, true);
  const chunks = [];
  let length = 0;
  stream.on('error', function(e) {
    throw e;
  });
  stream.on('data', function(c) {
    chunks.push(c);
    length += c.length;
  });
  stream.end(input);
  return Buffer.concat(chunks, length);
}

function decompressSync(input, params) {
  if (!Buffer.isBuffer(input)) {
    throw new Error('Brotli input is not a buffer.');
  }
  const stream = new TransformStreamDecode(params, true);
  const chunks = [];
  let length = 0;
  stream.on('error', function(e) {
    throw e;
  });
  stream.on('data', function(c) {
    chunks.push(c);
    length += c.length;
  });
  stream.end(input);
  return Buffer.concat(chunks, length);
}

function compressStream(params) {
  return new TransformStreamEncode(params);
}

function decompressStream(params) {
  return new TransformStreamDecode(params);
}

// We need to fill the blockSize for better compression results
function compressStreamChunk(stream, chunk, encoder, status, sync, done) {
  const length = chunk.length;

  if (length > status.remaining) {
    const slicedChunk = chunk.slice(0, status.remaining);
    chunk = chunk.slice(status.remaining);
    status.remaining = status.blockSize;

    encoder.copy(slicedChunk);
    encoder.encode(false, function(err, output) {
      if (err) {
        return done(err);
      }
      if (output) {
        for (let i = 0; i < output.length; i++) {
          stream.push(output[i]);
        }
      }
      compressStreamChunk(stream, chunk, encoder, status, sync, done);
    }, !sync);
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
        for (let i = 0; i < output.length; i++) {
          stream.push(output[i]);
        }
      }
      done();
    }, !sync);
  }
}
