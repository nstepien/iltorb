# iltorb

[![NPM Version][npm-badge]][npm-url]
[![Travis Build Status][travis-badge]][travis-url]
[![AppVeyor Build Status][appveyor-badge]][appveyor-url]
[![CircleCI Build Status][circleci-badge]][circleci-url]

[iltorb](https://www.npmjs.com/package/iltorb) is a [Node.js](https://nodejs.org) package offering native bindings for the [brotli](https://github.com/google/brotli) compression library.

## Install

This module uses `prebuild` to download a pre-compiled binary for your platform, if it exists. Otherwise, it will use `node-gyp` to build the module.

```
npm install iltorb
```

### Prerequisites for Building

The following is required to build from source or when a [pre-compiled binary](https://github.com/nstepien/iltorb/releases) does not exist.

- Python 2.7
- GCC 4.8+ (Unix) or [windows-build-tools](https://github.com/felixrieseberg/windows-build-tools) (Windows), see [Node Building tools](https://github.com/nodejs/node-gyp#installation).

## Methods

### Async

Omitting the callback argument will result in the compress and decompress methods to return a Promise.

#### compress(buffer[, brotliEncodeParams][, callback])

```javascript
const compress = require('iltorb').compress;

// callback style
compress(input, function(err, output) {
  // ...
});

// promise style
compress(input)
  .then(output => /* ... */)
  .catch(err => /* ... */);

// async/await style
try {
  const output = await compress(input);
} catch(err) {
  // ...
}
```

#### decompress(buffer[, callback])

```javascript
const decompress = require('iltorb').decompress;

// callback style
decompress(input, function(err, output) {
  // ...
});

// promise style
decompress(input)
  .then(output => /* ... */)
  .catch(err => /* ... */);

// async/await style
try {
  const output = await decompress(input);
} catch(err) {
  // ...
}
```

### Sync

#### compressSync(buffer[, brotliEncodeParams])

```javascript
const compressSync = require('iltorb').compressSync;

try {
  var output = compressSync(input);
} catch(err) {
  // ...
}
```

#### decompressSync(buffer)

```javascript
const decompressSync = require('iltorb').decompressSync;

try {
  var output = decompressSync(input);
} catch(err) {
  // ...
}
```

### Stream

#### compressStream([brotliEncodeParams])

```javascript
const compressStream = require('iltorb').compressStream;
const fs = require('fs');

fs.createReadStream('path/to/input')
  .pipe(compressStream())
  .pipe(fs.createWriteStream('path/to/output'));
```

##### compressionStream.flush()

Call this method to flush pending data. Don't call this frivolously, premature flushes negatively impact the effectiveness of the compression algorithm.

#### decompressStream()

```javascript
const decompressStream = require('iltorb').decompressStream;
const fs = require('fs');

fs.createReadStream('path/to/input')
  .pipe(decompressStream())
  .pipe(fs.createWriteStream('path/to/output'));
```

### brotliEncodeParams

The `compress`, `compressSync` and `compressStream` methods may accept an optional `brotliEncodeParams` object to define some or all of brotli's compression parameters:
- [type definition](https://github.com/google/brotli/blob/v1.0.4/c/enc/params.h#L30-L42)
- [defaults](https://github.com/google/brotli/blob/v1.0.4/c/enc/encode.c#L706-L720)
- [explanations](https://github.com/google/brotli/blob/v1.0.4/c/include/brotli/encode.h#L133-L205)

```javascript
const brotliEncodeParams = {
  mode: 0,
  quality: 11,
  lgwin: 22,
  lgblock: 0,
  disable_literal_context_modeling: false,
  size_hint: 0, // automatically set for `compress` and `compressSync`
  large_window: false,
  npostfix: 0,
  ndirect: 0
};
```

## Troubleshooting

1. I am unable to install `iltorb` because the host (GitHub) that serves the binaries is blocked by my firewall.

    a) By **default**, if the binaries could not be downloaded for any reason, the install script will attempt to compile the binaries locally on your machine. This requires having all of the build requirements fulfilled.

    b) You can override the `binary.host` value found in `package.json` with the following methods:

      - using the following ENV variable `npm_config_iltorb_binary_host=https://domain.tld/path`
      - as an additional argument with npm install `--iltorb_binary_host=https://domain.tld/path`

      Note: Both of these would result in downloading the binary from `https://domain.tld/path/vX.X.X/iltorb-vX.X.X-node-vXX-arch.tar.gz`


[npm-badge]: https://img.shields.io/npm/v/iltorb.svg
[npm-url]: https://www.npmjs.com/package/iltorb
[travis-badge]: https://img.shields.io/travis/nstepien/iltorb.svg
[travis-url]: https://travis-ci.org/nstepien/iltorb
[appveyor-badge]: https://ci.appveyor.com/api/projects/status/5x96vn5561bixics/branch/master?svg=true
[appveyor-url]: https://ci.appveyor.com/project/MayhemYDG/iltorb
[circleci-badge]: https://circleci.com/gh/nstepien/iltorb/tree/master.svg?style=shield
[circleci-url]: https://circleci.com/gh/nstepien/iltorb/tree/master
