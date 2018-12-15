import test from 'ava';
import * as brotli from '../';
import { readFileSync } from 'fs';
import { join } from 'path';

async function testBuffer(method, bufferFile, resultFile, t, params={}) {
  const buffer = readFileSync(join(__dirname, '/fixtures/', bufferFile));
  const result = readFileSync(join(__dirname, '/fixtures/', resultFile));

  const output = await (method.name === 'compress' ? method(buffer, params) : method(buffer));
  t.truthy(output.equals(result));
}

function testBufferError(method, t) {
  return t.throwsAsync(method('not a buffer'), {
    message: 'Brotli input is not a buffer.'
  });
}

test('compress binary data', function(t) {
  return testBuffer(brotli.compress, 'data10k.bin', 'data10k.bin.compressed', t);
});

test('compress text data', function(t) {
  return testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed', t);
});

test('compress text data with quality=3', function(t) {
  return testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed.03', t, { quality: 3 });
});

test('compress text data with quality=9', function(t) {
  return testBuffer(brotli.compress, 'data.txt', 'data.txt.compressed.09', t, { quality: 9 });
});

test('compress an empty buffer', function(t) {
  return testBuffer(brotli.compress, 'empty', 'empty.compressed', t);
});

test('compress a random buffer', function(t) {
  return testBuffer(brotli.compress, 'rand', 'rand.compressed', t);
});

test('compress a large buffer', function(t) {
  return testBuffer(brotli.compress, 'large.txt', 'large.txt.compressed', t);
});

test('reject with an error when the compression input is not a buffer', function(t) {
  return testBufferError(brotli.compress, t);
});

test('decompress binary data', function(t) {
  return testBuffer(brotli.decompress, 'data10k.bin.compressed', 'data10k.bin', t);
});

test('decompress text data', function(t) {
  return testBuffer(brotli.decompress, 'data.txt.compressed', 'data.txt', t);
});

test('decompress to an empty buffer', function(t) {
  return testBuffer(brotli.decompress, 'empty.compressed', 'empty', t);
});

test('decompress a random buffer', function(t) {
  return testBuffer(brotli.decompress, 'rand.compressed', 'rand', t);
});

test('decompress to a large buffer', function(t) {
  return testBuffer(brotli.decompress, 'large.compressed', 'large', t);
});

test('decompress to another large buffer', function(t) {
  return testBuffer(brotli.decompress, 'large.txt.compressed', 'large.txt', t);
});

test('reject with an error when the decompression input is not a buffer', function(t) {
  return testBufferError(brotli.decompress, t);
});
