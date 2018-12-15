import test from 'ava';
import * as brotli from '../';
import { readFileSync } from 'fs';
import { join } from 'path';

function testBuffer(method, bufferFile, resultFile, t, params={}) {
  const buffer = readFileSync(join(__dirname, '/fixtures/', bufferFile));
  const result = readFileSync(join(__dirname, '/fixtures/', resultFile));
  const output = method(buffer, params);
  t.truthy(output.equals(result));
}

function testBufferError(method, t) {
  t.throws(function() {
    method('not a buffer');
  }, {
    message: 'Brotli input is not a buffer.'
  });
}

test('compress binary data', function(t) {
  testBuffer(brotli.compressSync, 'data10k.bin', 'data10k.bin.compressed', t);
});

test('compress text data', function(t) {
  testBuffer(brotli.compressSync, 'data.txt', 'data.txt.compressed', t);
});

test('compress text data with quality=3', function(t) {
  testBuffer(brotli.compressSync, 'data.txt', 'data.txt.compressed.03', t, { quality: 3 });
});

test('compress text data with quality=9', function(t) {
  testBuffer(brotli.compressSync, 'data.txt', 'data.txt.compressed.09', t, { quality: 9 });
});

test('compress an empty buffer', function(t) {
  testBuffer(brotli.compressSync, 'empty', 'empty.compressed', t);
});

test('compress a random buffer', function(t) {
  testBuffer(brotli.compressSync, 'rand', 'rand.compressed', t);
});

test('compress a large buffer', function(t) {
  testBuffer(brotli.compressSync, 'large.txt', 'large.txt.compressed', t);
});

test('throw when the compression input is not a buffer', function(t) {
  testBufferError(brotli.compressSync, t);
});

test('decompress binary data', function(t) {
  testBuffer(brotli.decompressSync, 'data10k.bin.compressed', 'data10k.bin', t);
});

test('decompress text data', function(t) {
  testBuffer(brotli.decompressSync, 'data.txt.compressed', 'data.txt', t);
});

test('decompress to an empty buffer', function(t) {
  testBuffer(brotli.decompressSync, 'empty.compressed', 'empty', t);
});

test('decompress a random buffer', function(t) {
  testBuffer(brotli.decompressSync, 'rand.compressed', 'rand', t);
});

test('decompress to a large buffer', function(t) {
  testBuffer(brotli.decompressSync, 'large.compressed', 'large', t);
});

test('decompress to another large buffer', function(t) {
  testBuffer(brotli.decompressSync, 'large.txt.compressed', 'large.txt', t);
});

test('throw when the decompression input is not a buffer', function(t) {
  testBufferError(brotli.decompressSync, t);
});
