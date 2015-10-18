'use strict';

const brotli = require('../');
const expect = require('expect.js');
const Writable = require('stream').Writable;
const fs = require('fs');

function testStream(method, bufferFile, resultFile, done) {
  let data;
  const writeStream = new Writable({
    write: function(chunk, encoding, next) {
      if (data) {
        data = Buffer.concat([data, chunk], data.length + chunk.length);
      } else {
        data = chunk;
      }
      next();
    }
  });

  fs.createReadStream(`${__dirname}/fixtures/${bufferFile}`)
    .pipe(method())
    .pipe(writeStream);

  writeStream.on('finish', function() {
    const result = fs.readFileSync(`${__dirname}/fixtures/${resultFile}`);
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
  });

  describe('decompress', function() {
    it('should compress binary data', function(done) {
      testStream(brotli.decompressStream, 'data10k.bin.compressed', 'data10k.bin', done);
    });

    it('should compress text data', function(done) {
      testStream(brotli.decompressStream, 'data.txt.compressed', 'data.txt', done);
    });
  });
});
