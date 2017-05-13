'use strict';

const brotli = require('../');
const chai = require('chai');
const expect = chai.expect;
const Writable = require('stream').Writable;
const fs = require('fs');
const path = require('path');

class BufferWriter extends Writable {
  constructor() {
    super();
    this.data = new Buffer(0);
  }

  _write(chunk, encoding, next) {
    this.data = Buffer.concat([this.data, chunk], this.data.length + chunk.length);
    next();
  }
}

function testStream(method, bufferFile, resultFile, done, params) {
  const writeStream = new BufferWriter();

  fs.createReadStream(path.join(__dirname, '/fixtures/', bufferFile))
    .pipe(method(params))
    .pipe(writeStream);

  writeStream.on('finish', function() {
    const result = fs.readFileSync(path.join(__dirname, '/fixtures/', resultFile));
    expect(writeStream.data).to.deep.equal(result);
    done();
  });
}

describe('Brotli Stream', function() {
  describe('compress', function() {
    it('should compress binary data', function(done) {
      testStream(brotli.compressStream, 'data10k.bin', 'data10k.bin.compressed', done);
    });

    it('should compress binary data', function(done) {
      testStream(brotli.compressStream, 'data10k.bin', 'data10k.bin.compressed.dict', done, { dictionary: Buffer.from('0123456789') });
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
      this.timeout(30000);
      testStream(brotli.compressStream, 'large.txt', 'large.txt.compressed', done);
    });

    it('should flush data', function(done) {
      const buf1 = new Buffer('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.');
      const buf2 = new Buffer('Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

      const stream = brotli.compressStream();
      const writeStream = new BufferWriter();

      stream
        .pipe(brotli.decompressStream())
        .pipe(writeStream);

      stream.once('data', function() {
        stream.end(buf2);
      });
      stream.write(buf1);
      stream.flush();

      writeStream.on('finish', function() {
        expect(writeStream.data.toString()).to.equal(Buffer.concat([buf1, buf2]).toString());
        done();
      });
    });
  });

  describe('decompress', function() {
    it('should decompress binary data', function(done) {
      testStream(brotli.decompressStream, 'data10k.bin.compressed', 'data10k.bin', done);
    });

    it('should decompress binary data', function(done) {
      testStream(brotli.decompressStream, 'data10k.bin.compressed.dict', 'data10k.bin', done, { dictionary: Buffer.from('0123456789') });
    });

    it('should decompress text data', function(done) {
      testStream(brotli.decompressStream, 'data.txt.compressed', 'data.txt', done);
    });

    it('should decompress to an empty buffer', function(done) {
      testStream(brotli.decompressStream, 'empty.compressed', 'empty', done);
    });

    it('should decompress to a large buffer', function(done) {
      this.timeout(30000);
      testStream(brotli.decompressStream, 'large.compressed', 'large', done);
    });

    it('should decompress to another large buffer', function(done) {
      this.timeout(30000);
      testStream(brotli.decompressStream, 'large.txt.compressed', 'large.txt', done);
    });
  });
});
