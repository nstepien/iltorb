'use strict';

const brotli = require('../');
const expect = require('expect.js');
const fs = require('fs');

function testBufferAsync(method, bufferFile, resultFile, done, params) {
  params = params || {};
  const buffer = fs.readFileSync(`${__dirname}/fixtures/${bufferFile}`);
  const result = fs.readFileSync(`${__dirname}/fixtures/${resultFile}`);

  if (method.name === 'compress') {
    method(buffer, params, function(err, output) {
      expect(output).to.eql(result);
      done();
    });
  }

  if (method.name === 'decompress') {
    method(buffer, function(err, output) {
      expect(output).to.eql(result);
      done();
    });
  }
}

describe('Brotli Buffer Async', function() {
  describe('compress', function() {
    it('should compress binary data', function(done) {
      testBufferAsync(brotli.compress, 'data10k.bin', 'data10k.bin.compressed', done);
    });

    it('should compress text data', function(done) {
      testBufferAsync(brotli.compress, 'data.txt', 'data.txt.compressed', done);
    });

    it('should compress text data with quality=3', function(done) {
      testBufferAsync(brotli.compress, 'data.txt', 'data.txt.compressed.03', done, { quality: 3 });
    });

    it('should compress text data with quality=9', function(done) {
      testBufferAsync(brotli.compress, 'data.txt', 'data.txt.compressed.09', done, { quality: 9 });
    });

    it('should compress an empty buffer', function(done) {
      testBufferAsync(brotli.compress, 'empty', 'empty.compressed', done);
    });
  });

  describe('decompress', function() {
    it('should decompress binary data', function(done) {
      testBufferAsync(brotli.decompress, 'data10k.bin.compressed', 'data10k.bin', done);
    });

    it('should decompress text data', function(done) {
      testBufferAsync(brotli.decompress, 'data.txt.compressed', 'data.txt', done);
    });

    it('should decompress to an empty buffer', function(done) {
      testBufferAsync(brotli.decompress, 'empty.compressed', 'empty', done);
    });
  });
});
