import test from 'ava';
import * as brotli from '../';
import { Writable } from 'stream';
import { createReadStream, readFileSync } from 'fs';
import { join } from 'path';

class BufferWriter extends Writable {
  constructor() {
    super();
    this.chunks = [];
    this.size = 0;
  }

  _write(chunk, encoding, next) {
    this.chunks.push(chunk);
    this.size += chunk.length;
    next();
  }

  get data() {
    return Buffer.concat(this.chunks, this.size);
  }
}

function testStream(method, bufferFile, resultFile, t, params) {
  const writeStream = new BufferWriter();

  createReadStream(join(__dirname, '/fixtures/', bufferFile))
    .pipe(method(params))
    .pipe(writeStream);

  writeStream.on('finish', function() {
    const result = readFileSync(join(__dirname, '/fixtures/', resultFile));
    t.truthy(writeStream.data.equals(result));
    t.end();
  });
}

test.cb('compress binary data', function(t) {
  testStream(brotli.compressStream, 'data10k.bin', 'data10k.bin.compressed', t);
});

test.cb('compress text data', function(t) {
  testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed', t);
});

test.cb('compress text data with quality=3', function(t) {
  testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed.03', t, { quality: 3 });
});

test.cb('compress text data with quality=9', function(t) {
  testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed.09', t, { quality: 9 });
});

test.cb('compress an empty buffer', function(t) {
  testStream(brotli.compressStream, 'empty', 'empty.compressed', t);
});

test.cb('compress a random buffer', function(t) {
  testStream(brotli.compressStream, 'rand', 'rand.compressed', t);
});

test.cb('compress a large buffer', function(t) {
  testStream(brotli.compressStream, 'large.txt', 'large.txt.compressed', t);
});

test.cb('flush data', function(t) {
  const buf1 = Buffer.from('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.');
  const buf2 = Buffer.from('Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

  const stream = brotli.compressStream();
  const writeStream = new BufferWriter();

  stream
    .pipe(brotli.decompressStream())
    .pipe(writeStream);

  stream.write(buf1);
  stream.flush();
  stream.once('data', function() {
    stream.end(buf2);
  });

  writeStream.on('finish', function() {
    t.truthy(writeStream.data.equals(Buffer.concat([buf1, buf2])));
    t.end();
  });
});

test.cb('decompress binary data', function(t) {
  testStream(brotli.decompressStream, 'data10k.bin.compressed', 'data10k.bin', t);
});

test.cb('decompress text data', function(t) {
  testStream(brotli.decompressStream, 'data.txt.compressed', 'data.txt', t);
});

test.cb('decompress to an empty buffer', function(t) {
  testStream(brotli.decompressStream, 'empty.compressed', 'empty', t);
});

test.cb('decompress to a random buffer', function(t) {
  testStream(brotli.decompressStream, 'rand.compressed', 'rand', t);
});

test.cb('decompress to a large buffer', function(t) {
  testStream(brotli.decompressStream, 'large.compressed', 'large', t);
});

test.cb('decompress to another large buffer', function(t) {
  testStream(brotli.decompressStream, 'large.txt.compressed', 'large.txt', t);
});
