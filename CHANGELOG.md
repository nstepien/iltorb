# Change Log
All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Semantic Versioning](http://semver.org/).

## [2.4.5] - 2020-02-01
- `iltorb` is now deprecated in favor of `zlib`.
  - The `zlib` module has support for brotli compression/decompression APIs starting with [Node.js v10.16.0](https://nodejs.org/dist/latest/docs/api/zlib.html).

## [2.4.4] - 2019-10-25
### Added
- Support for Node 13

### Removed
- Dropped support for Node 6 and Node 11

## [2.4.3] - 2019-04-24
### Added
- Support for Node 12

### Changed
- Updated npm dependencies.

## [2.4.2] - 2019-03-11
### Changed
- Moved back to using NAN from N-API to hopefully fix some segmentation faults.

## [2.4.1] - 2018-10-31
### Added
- Support for Node 11

### Fixed
- Fixed `Module did not self-register` errors, happening when `require()`-ing iltorb multiple times, with worker threads for example. #78

### Changed
- Updated brotli from [v1.0.5] to [v1.0.7]

## [2.4.0] - 2018-07-31
### Changed
- Updated brotli from [v1.0.4] to [v1.0.5]
- Replaced NAN with N-API

### Removed
- Dropped support for Node 9

## [2.3.2] - 2018-05-01
### Fixed
- Fixed pre-compiled binaries download script

## [2.3.1] - 2018-04-30
### Added
- Support for Node 10

### Removed
- Dropped support for Node 4

## [2.3.0] - 2018-04-06
### Added
- The compress and decompress methods will now return a promise when the callback argument is missing.
  - This change enables using those methods in async/await flow.

## [2.2.0] - 2018-03-30
### Changed
- Updated brotli from [v1.0.3] to [v1.0.4]

### Added
- Support for new brotli compression parameters:
  - `large_window`
  - `npostfix`
  - `ndirect`

### Fixed
- brotli parameter `size_hint` was being parsed as a boolean instead of an integer.

## [2.1.0] - 2018-03-24
### Changed
- Reduced package size by ~9M

### Removed
- Use bundled `node-gyp` provided by package managers instead

## [2.0.9] - 2018-03-23
### Fixed
- Build dependencies were incorrectly fixed

## [2.0.8] - 2018-03-23
### Fixed
- Missing dependency was added back

## [2.0.7] - 2018-03-23
### Fixed
- Install process on non-npm package managers

## [2.0.6] - 2018-03-16
### Added
- Support for `--no-bin-link`

### Changed
- Moved `build.js` to `scripts` directory

## [2.0.5] - 2018-03-12
### Removed
- Workaround for `--no-bin-link`

## [2.0.4] - 2018-03-11
### Changed
- Updated brotli from [v1.0.2] to [v1.0.3]

### Fixed
- Fixed installation with `--no-bin-links`

## [2.0.3] - 2017-12-02
### Changed
- Updated brotli from [v1.0.1] to [v1.0.2]

## [2.0.2] - 2017-10-24
### Fixed
- Link failure on Android

## [2.0.1] - 2017-10-16
### Added
- Added CircleCI status badge to README
- Support Linux distributions both glibc and musql systems with pre-compiled binaries for both

### Changed
- Replace Travis-CI with CircleCI for Linux platforms

## [2.0.0] - 2017-10-12
### Changed
- Updated brotli from [v0.6.0] to [v1.0.1]

### Removed
- Removed custom dictionary support

## [1.3.10] - 2017-10-10
### Fixed
- Revert `package.json` change which removed the `binary` key containing remote path and host

## [1.3.9] - 2017-10-09
### Fixed
- Revert `package.json` change which removed the `binary` key containing module name and path
- Update CI to build all supported versions
- Update Appyveyor to properly upload binaries

## [1.3.8] - 2017-10-08
### Removed
- README no longer contains a TROUBLESHOOT section

## [1.3.7] - 2017-10-08
### Changed
- Replaced `node-pre-gyp` with `prebuild`
- Update build pipeline to output single binary

## [1.3.6] - 2017-08-30
### Changed
- Removed workaround for distros with old libc versions

### Fixed
- Infinite compression loop

### Removed
- Drop support for Node 7

## [1.3.5] - 2017-07-18
### Changed
- Update CI build pipeline to publish to GitHub instead of S3

## [1.3.4] - 2017-07-09
### Fixed
- Link failure on ARM

## [1.3.3] - 2017-07-04
### Fixed
- Link failure on ARM

## [1.3.2] - 2017-07-01
### Changed
- Support Linux distributions with older glibc versions via memcpy patch

## [1.3.1] - 2017-05-31
### Changed
- Published pre-compiled binaries for Node 8

## [1.3.0] - 2017-05-14
### Added
- CHANGELOG
- Support `size_hint` and `disable_literal_context_modeling` encode parameters
- Support for Windows x86

### Changed
- Update CI build configs to use `JOBS=max`
- Update Brotli dependency to [brotli-0.6.0]

## [1.2.1] - 2017-04-12
### Added
- README contains link to pre-compiled binaries and information on building prerequisites

### Changed
- Update CI to publish on tags only

## [1.2.0] - 2017-04-11
### Added
- Support for the `flush()` method on compression streams

## [1.1.0] - 2017-04-09
### Added
- Support for pre-built binaries on supported platforms
- Support for custom directory with decoder

### Changed
- Update CI build matrix to test with Node v7.x
- Update link to Brotli compression settings
- Update NPM dependencies: `nan` to `2.6.1`
- Update syntax to ES2015
- Replace `expect.js` with `chai`

### Removed
- Drop support for Node v0.10, Node v0.12, and Node v5.x
- Drop support for Windows x86

## [1.0.13] - 2016-10-08
### Added
- README contains information about windows build tools

### Changed
- Update Brotli dependency to [brotli-0.5.2]

## [1.0.12] - 2016-06-18
### Changed
- Increase decode output buffer size to improve decoding performance

## [1.0.11] - 2016-06-17
### Fixed
- Add missing `brotli/common` files to package

## [1.0.10] - 2016-06-17
### Changed
- Increase timeout for tests that work with large input buffers
- Update Brotli dependency to [brotli-0.5.0]
- Update CI build matrix to test with Node v6.x

## [1.0.9] - 2016-02-09
### Added
- Support for Windows

### Changed
- Update link to Brotli compression settings
- Update Brotli dependency to [brotli-0.3.0]

## [1.0.8] - 2016-01-31
### Changed
- Update Travis CI build matrix to test with Node v0.10

### Removed
- `constructor.Reset()` calls in destructors

## [1.0.7] - 2015-11-04
### Added
- `DecodeWorker`, `StreamDecode`, `StreamDecodeWorker` destructor
- `EncodeWorker`, `StreamEncodeWorker` destructor
- `constructor.Reset()` calls in destructors

### Changed
- Update constructors to use most up-to-date NAN ObjectWrap

## [1.0.6] - 2015-11-03
### Added
- `StreamEncode` destructor

### Changed
- Update Travis CI build matrix to test with Node v5.x

## [1.0.5] - 2015-10-27
### Added
- Support for older implementation of streams

### Changed
- Downgrade to ES5
- Update Travis CI build matrix to test with Node v0.12
- Update Brotli dependency to [brotli@8523d36]

## [1.0.4] - 2015-10-24
### Added
- Unbuffered streaming decompression
- Test for compression parameters with streams

### Changed
- Restructure encoder/decoder code into their own subdirectories
- Move `BufferOut`, `EncoderWorker`, `DecodeWorker` and brotli buffer output functions into their own files
- Update `cflags` to ignore `-Wsign-compare` warnings
- Update Brotli dependency to [brotli@87281b1]

## [1.0.3] - 2015-10-19
### Added
- Unbuffered streaming compression

## [1.0.2] - 2015-10-19
### Changed
- Update Brotli dependency to [brotli@20e838f]

### Fixed
- Handle large input buffers properly

## [1.0.1] - 2015-10-19
### Added
- README now contains badges to NPM and Travis CI

### Fixed
- Fix transform streams to properly handle empty input
- Update Travis CI configuration file to properly compile

## [1.0.0] - 2015-10-18

[2.4.5]: https://github.com/nstepien/iltorb/compare/v2.4.4...v2.4.5
[2.4.4]: https://github.com/nstepien/iltorb/compare/v2.4.3...v2.4.4
[2.4.3]: https://github.com/nstepien/iltorb/compare/v2.4.2...v2.4.3
[2.4.2]: https://github.com/nstepien/iltorb/compare/v2.4.1...v2.4.2
[2.4.1]: https://github.com/nstepien/iltorb/compare/v2.4.0...v2.4.1
[2.4.0]: https://github.com/nstepien/iltorb/compare/v2.3.2...v2.4.0
[2.3.2]: https://github.com/nstepien/iltorb/compare/v2.3.1...v2.3.2
[2.3.1]: https://github.com/nstepien/iltorb/compare/v2.3.0...v2.3.1
[2.3.0]: https://github.com/nstepien/iltorb/compare/v2.2.0...v2.3.0
[2.2.0]: https://github.com/nstepien/iltorb/compare/v2.1.0...v2.2.0
[2.1.0]: https://github.com/nstepien/iltorb/compare/v2.0.9...v2.1.0
[2.0.9]: https://github.com/nstepien/iltorb/compare/v2.0.8...v2.0.9
[2.0.8]: https://github.com/nstepien/iltorb/compare/v2.0.7...v2.0.8
[2.0.7]: https://github.com/nstepien/iltorb/compare/v2.0.6...v2.0.7
[2.0.6]: https://github.com/nstepien/iltorb/compare/v2.0.5...v2.0.6
[2.0.5]: https://github.com/nstepien/iltorb/compare/v2.0.4...v2.0.5
[2.0.4]: https://github.com/nstepien/iltorb/compare/v2.0.3...v2.0.4
[2.0.3]: https://github.com/nstepien/iltorb/compare/v2.0.2...v2.0.3
[2.0.2]: https://github.com/nstepien/iltorb/compare/v2.0.1...v2.0.2
[2.0.1]: https://github.com/nstepien/iltorb/compare/v2.0.0...v2.0.1
[2.0.0]: https://github.com/nstepien/iltorb/compare/v1.3.10...v2.0.0
[1.3.10]: https://github.com/nstepien/iltorb/compare/v1.3.9...v1.3.10
[1.3.9]: https://github.com/nstepien/iltorb/compare/v1.3.8...v1.3.9
[1.3.8]: https://github.com/nstepien/iltorb/compare/v1.3.7...v1.3.8
[1.3.7]: https://github.com/nstepien/iltorb/compare/1.3.6...v1.3.7
[1.3.6]: https://github.com/nstepien/iltorb/compare/1.3.5...1.3.6
[1.3.5]: https://github.com/nstepien/iltorb/compare/1.3.4...1.3.5
[1.3.4]: https://github.com/nstepien/iltorb/compare/1.3.3...1.3.4
[1.3.3]: https://github.com/nstepien/iltorb/compare/1.3.2...1.3.3
[1.3.2]: https://github.com/nstepien/iltorb/compare/1.3.1...1.3.2
[1.3.1]: https://github.com/nstepien/iltorb/compare/1.3.0...1.3.1
[1.3.0]: https://github.com/nstepien/iltorb/compare/1.2.1...1.3.0
[1.2.1]: https://github.com/nstepien/iltorb/compare/1.2.0...1.2.1
[1.2.0]: https://github.com/nstepien/iltorb/compare/1.1.0...1.2.0
[1.1.0]: https://github.com/nstepien/iltorb/compare/1.0.13...1.1.0
[1.0.13]: https://github.com/nstepien/iltorb/compare/1.0.12...1.0.13
[1.0.12]: https://github.com/nstepien/iltorb/compare/1.0.11...1.0.12
[1.0.11]: https://github.com/nstepien/iltorb/compare/1.0.10...1.0.11
[1.0.10]: https://github.com/nstepien/iltorb/compare/1.0.9...1.0.10
[1.0.9]: https://github.com/nstepien/iltorb/compare/1.0.8...1.0.9
[1.0.8]: https://github.com/nstepien/iltorb/compare/1.0.7...1.0.8
[1.0.7]: https://github.com/nstepien/iltorb/compare/1.0.6...1.0.7
[1.0.6]: https://github.com/nstepien/iltorb/compare/1.0.5...1.0.6
[1.0.5]: https://github.com/nstepien/iltorb/compare/1.0.4...1.0.5
[1.0.4]: https://github.com/nstepien/iltorb/compare/1.0.3...1.0.4
[1.0.3]: https://github.com/nstepien/iltorb/compare/1.0.2...1.0.3
[1.0.2]: https://github.com/nstepien/iltorb/compare/1.0.1...1.0.2
[1.0.1]: https://github.com/nstepien/iltorb/compare/1.0.0...1.0.1
[1.0.1]: https://github.com/nstepien/iltorb/releases/tag/1.0.0

[v1.0.7]: https://github.com/google/brotli/releases/tag/v1.0.7
[v1.0.5]: https://github.com/google/brotli/releases/tag/v1.0.5
[v1.0.4]: https://github.com/google/brotli/releases/tag/v1.0.4
[v1.0.3]: https://github.com/google/brotli/releases/tag/v1.0.3
[v1.0.2]: https://github.com/google/brotli/releases/tag/v1.0.2
[v1.0.1]: https://github.com/google/brotli/releases/tag/v1.0.1
[v0.6.0]: https://github.com/google/brotli/releases/tag/v0.6.0
[brotli-0.6.0]: https://github.com/google/brotli/releases/tag/v0.6.0
[brotli-0.5.2]: https://github.com/google/brotli/releases/tag/v0.5.2
[brotli-0.5.0]: https://github.com/google/brotli/tree/v0.5.0
[brotli-0.3.0]: https://github.com/google/brotli/tree/v0.3.0
[brotli@8523d36]: https://github.com/google/brotli/tree/8523d36e698eced028b938c834d38a89d3988caa
[brotli@87281b1]: https://github.com/google/brotli/tree/87281b127cbb2560ccf18ef5d2018055cff3dfc2
[brotli@20e838f]: https://github.com/google/brotli/tree/20e838f6adf5f337671aeff38ee757938c556569
