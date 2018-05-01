#!/usr/bin/env node

/**
 * Modified Source: `prebuild-install`
 * https://github.com/prebuild/prebuild-install/blob/master/bin.js
 *
 * This install wrapper is written to provide support for the `--no-bin-links`
 * install option provided by npm and yarn. It takes advantage of the module
 * resolution logic to run instead of relying on the PATH env to perform the
 * installation.
 */

const path = require('path');
const prebuildRC = require('prebuild-install/rc');
const prebuildDL = require('prebuild-install/download');
const prebuildUtil = require('prebuild-install/util');
const whichPM = require('which-pm-runs');
const log = require('npmlog');
const pkg = require(path.resolve(__dirname, '../package.json'));

const conf = prebuildRC(pkg);
const opts = Object.assign({}, conf, { pkg, log });

const pm = whichPM();
const npm = !pm || pm.name === 'npm';

if (conf.verbose) {
  log.level = 'verbose';
}

if (opts.force) {
  log.warn('install', 'prebuilt binaries enforced with --force!')
  log.warn('install', 'prebuilt binaries may be out of date!')
} else if (!npm && /node_modules/.test(process.cwd())) {
  // This is a NOOP conditional that is used to handle certain situations where
  // the PM may build from source instead of attempting to download the binary.
} else if (!(typeof pkg._from === 'string')) {
  log.info('install', 'installing standalone, skipping download.')
  process.exit(1)
} else if (pkg._from.length > 4 && pkg._from.substr(0, 4) === 'git+') {
  log.info('install', 'installing from git repository, skipping download.')
  process.exit(1)
} else if (opts.compile === true || opts.prebuild === false) {
  log.info('install', '--build-from-source specified, not attempting download.')
  process.exit(1)
}

prebuildDL(prebuildUtil.getDownloadUrl(opts), opts, function(err) {
  if (err) {
    log.warn('install', err.message);
    process.exit(1);
  }

  log.info('install', 'Successfully installed iltorb binary!');
});
