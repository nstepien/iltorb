import test from 'ava';
import * as brotli from '../';
import { readFileSync } from 'fs';
import { join } from 'path';

function testBuffer(method, bufferFile, resultFile, t, params={}) {
  const buffer = readFileSync(join(__dirname, '/fixtures/', bufferFile));
  const result = readFileSync(join(__dirname, '/fixtures/', resultFile));

  function cb(err, output) {
    t.truthy(output.equals(result));
    t.end(err);
  }

  if (method.name === 'compress') {
    method(buffer, params, cb);
  }

  if (method.name === 'decompress') {
    method(buffer, cb);
  }
}

function testBufferError(method, t) {
  return t.throwsAsync(new Promise(function(resolve, reject) {
    method('not a buffer', function(err) {
      err ? reject(err) : resolve();
    });
  }), {
    message: 'Brotli input is not a buffer.'
  });
}

test.cb('compress binary data', function(t) {
  testBuffer(brotli.compress, 'data10k.bin', 'data10k.bin.compressed', t);
});

test.cb('compress text data', function(t) {
  testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed', t);
});

test.cb('compress text data with quality=3', function(t) {
  testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed.03', t, { quality: 3 });
});

test.cb('compress text data with quality=9', function(t) {
  testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed.09', t, { quality: 9 });
});

test.cb('compress an empty buffer', function(t) {
  testBuffer(brotli.compress, 'empty', 'empty.compressed', t);
});

test.cb('compress a random buffer', function(t) {
  testBuffer(brotli.compress, 'rand', 'rand.compressed', t);
});

test.cb('compress a large buffer', function(t) {
  testBuffer(brotli.compress, 'large.txt', 'large.txt.compressed', t);
});

test('call back with an error when the compress input is not a buffer', function(t) {
  return testBufferError(brotli.compress, t);
});

test.cb('decompress binary data', function(t) {
  testBuffer(brotli.decompress, 'data10k.bin.compressed', 'data10k.bin', t);
});

test.cb('decompress text data', function(t) {
  testBuffer(brotli.decompress, 'data.txt.compressed', 'data.txt', t);
});

test.cb('decompress to an empty buffer', function(t) {
  testBuffer(brotli.decompress, 'empty.compressed', 'empty', t);
});

test.cb('decompress a random buffer', function(t) {
  testBuffer(brotli.decompress, 'rand.compressed', 'rand', t);
});

test.cb('decompress to a large buffer', function(t) {
  testBuffer(brotli.decompress, 'large.compressed', 'large', t);
});

test.cb('decompress to another large buffer', function(t) {
  testBuffer(brotli.decompress, 'large.txt.compressed', 'large.txt', t);
});

test('call back with an error when the decompression input is not a buffer', function(t) {
  return testBufferError(brotli.decompress, t);
});
