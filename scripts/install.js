const path = require('path');
const libc = require('detect-libc');
const prebuildRC = require('prebuild-install/rc');
const prebuildDL = require('prebuild-install/download');
const log = require('npmlog');
const pkg = require(path.resolve(__dirname, '../package.json'));

const opts = Object.assign({}, prebuildRC(pkg), { pkg, log });

prebuildDL(opts, function(err) {
  if (err) {
    log.warn('install', err.message);
    process.exit(1);
  }

  log.info('install', 'Successfully installed iltorb binary!');
});
