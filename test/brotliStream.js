'use strict';

var brotli = require('../');
var expect = require('expect.js');
var Writable = require('stream').Writable;
var fs = require('fs');

function testStream(method, bufferFile, resultFile, done, params) {
  var data = new Buffer(0);
  var writeStream = new Writable({
    write: function(chunk, encoding, next) {
      data = Buffer.concat([data, chunk], data.length + chunk.length);
      next();
    }
  });

  fs.createReadStream(__dirname + '/fixtures/' + bufferFile)
    .pipe(method(params))
    .pipe(writeStream);

  writeStream.on('finish', function() {
    var result = fs.readFileSync(__dirname + '/fixtures/' + resultFile);
    expect(data).to.eql(result);
    done();
  });
}

describe('Brotli Stream', function() {
  describe('compress', function() {
    it('should compress binary data', function(done) {
      testStream(brotli.compressStream, 'data10k.bin', 'data10k.bin.compressed', done);
    });

    it('should compress text data', function(done) {
      testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed', done);
    });

    it('should compress text data with quality=3', function(done) {
      testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed.03', done, { quality: 3 });
    });

    it('should compress text data with quality=9', function(done) {
      testStream(brotli.compressStream, 'data.txt', 'data.txt.compressed.09', done, { quality: 9 });
    });

    it('should compress an empty buffer', function(done) {
      testStream(brotli.compressStream, 'empty', 'empty.compressed', done);
    });

    it('should compress a large buffer', function(done) {
      testStream(brotli.compressStream, 'large.txt', 'large.txt.compressed', done);
    });
  });

  describe('decompress', function() {
    it('should decompress binary data', function(done) {
      testStream(brotli.decompressStream, 'data10k.bin.compressed', 'data10k.bin', done);
    });

    it('should decompress text data', function(done) {
      testStream(brotli.decompressStream, 'data.txt.compressed', 'data.txt', done);
    });

    it('should decompress to an empty buffer', function(done) {
      testStream(brotli.decompressStream, 'empty.compressed', 'empty', done);
    });

    it('should decompress to a large buffer', function(done) {
      testStream(brotli.decompressStream, 'large.compressed', 'large', done);
    });

    it('should decompress to another large buffer', function(done) {
      testStream(brotli.decompressStream, 'large.txt.compressed', 'large.txt', done);
    });
  });
});
