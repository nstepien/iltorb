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
    this.flushing = false;
    this.encoder = new encode.StreamEncode(params || {});
    const blockSize = this.encoder.getBlockSize();
    this.status = {
      blockSize,
      remaining: blockSize
    };
  }

  // We need to fill the blockSize for better compression results
  _transform(chunk, encoding, next) {
    const status = this.status;
    const length = chunk.length;

    if (length > status.remaining) {
      const slicedChunk = chunk.slice(0, status.remaining);
      chunk = chunk.slice(status.remaining);
      status.remaining = status.blockSize;

      this.encoder.copy(slicedChunk);
      this.encoder.encode(false, (err, output) => {
        if (err) {
          return next(err);
        }
        this._push(output);
        this._transform(chunk, encoding, next);
      }, !this.sync);
    } else if (length < status.remaining) {
      status.remaining -= length;
      this.encoder.copy(chunk);
      next();
    } else { // length === status.remaining
      status.remaining = status.blockSize;
      this.encoder.copy(chunk);
      this.encoder.encode(false, (err, output) => {
        if (err) {
          return next(err);
        }
        this._push(output);
        next();
      }, !this.sync);
    }
  }

  _flush(done) {
    this.encoder.encode(true, (err, output) => {
      if (err) {
        return done(err);
      }
      this._push(output);
      done();
    }, !this.sync);
  }

  _push(output) {
    if (output) {
      for (let i = 0; i < output.length; i++) {
        this.push(output[i]);
      }
    }
  }

  flush() {
    if (this.flushing) {
      return;
    }

    this.cork();
    this.flushing = true;

    this.encoder.flush((err, output) => {
      if (err) {
        this.emit('error', err);
      } else {
        this.status.remaining = this.status.blockSize;
        this._push(output);
      }
      this.uncork();
      this.flushing = false;
    });
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
      this._push(output);
      next();
    }, !this.sync);
  }

  _flush(done) {
    this.decoder.flush((err, output) => {
      if (err) {
        return done(err);
      }
      this._push(output);
      done();
    }, !this.sync);
  }

  _push(output) {
    if (output) {
      for (let i = 0; i < output.length; i++) {
        this.push(output[i]);
      }
    }
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
