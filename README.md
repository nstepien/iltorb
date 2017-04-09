# iltorb

[![NPM Version][npm-badge]][npm-url]
[![Travis Build Status][travis-badge]][travis-url]
[![AppVeyor Build Status][appveyor-badge]][appveyor-url]

[iltorb](https://www.npmjs.com/package/iltorb) is a [Node.js](https://nodejs.org) package offering native bindings for the [brotli](https://github.com/google/brotli) compression library.

## Usage

### Install

On Windows, in order to compile the native bindings, it is recommended to install the [windows-build-tools](https://github.com/felixrieseberg/windows-build-tools) beforehand.

```
npm install iltorb
```

### Async

#### compress(buffer[, brotliParams], callback)

```javascript
const compress = require('iltorb').compress;

compress(input, function(err, output) {
  // ...
});
```

#### decompress(buffer[, brotliDecodeParams], callback)

```javascript
const decompress = require('iltorb').decompress;

decompress(input, function(err, output) {
  // ...
});
```

### Sync

#### compressSync(buffer[, brotliParams])

```javascript
const compressSync = require('iltorb').compressSync;

try {
  var output = compressSync(input);
} catch(err) {
  // ...
}
```

#### decompressSync(buffer[, brotliDecodeParams])

```javascript
const decompressSync = require('iltorb').decompressSync;

try {
  var output = decompressSync(input);
} catch(err) {
  // ...
}
```

### Stream

#### compressStream([brotliParams])

```javascript
const compressStream = require('iltorb').compressStream;
const fs = require('fs');

fs.createReadStream('path/to/input')
  .pipe(compressStream())
  .pipe(fs.createWriteStream('path/to/output'));
```

#### decompressStream([brotliDecodeParams])

```javascript
const decompressStream = require('iltorb').decompressStream;
const fs = require('fs');

fs.createReadStream('path/to/input')
  .pipe(decompressStream())
  .pipe(fs.createWriteStream('path/to/output'));
```

### brotliParams

The `compress`, `compressSync` and `compressStream` methods may accept an optional `brotliParams` object to define some or all of [brotli's compression settings](https://github.com/google/brotli/blob/66c14517cf8afcc1a1649a7833ac789366eb0b51/enc/encode.h#L50-L60).

```javascript
const brotliParams = {
  mode: 0,
  quality: 11,
  lgwin: 22,
  lgblock: 0,
  dictionary: Buffer
};
```

### brotliDecodeParams

The `decompress`, `decompressSync` and `decompressStream` methods may accept an optional `brotliDecodeParams` object to provide a custom dictionary.

```javascript
const brotliDecodeParams = {
  dictionary: Buffer
};
```

[npm-badge]: https://img.shields.io/npm/v/iltorb.svg
[npm-url]: https://www.npmjs.com/package/iltorb
[travis-badge]: https://img.shields.io/travis/MayhemYDG/iltorb.svg
[travis-url]: https://travis-ci.org/MayhemYDG/iltorb
[appveyor-badge]: https://ci.appveyor.com/api/projects/status/ysib4o1bfey84lqk/branch/master?svg=true
[appveyor-url]: https://ci.appveyor.com/project/MayhemYDG/iltorb
