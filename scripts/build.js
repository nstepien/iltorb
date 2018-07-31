const libc = require('detect-libc');
const getTarget = require('node-abi').getTarget;
const exec = require('child_process').exec;
const spawn = require('cross-spawn');
const npmRunPath = require('npm-run-path-compat');

const PREBUILD_TOKEN = process.env.PREBUILD_TOKEN;
const PUBLISH_BINARY = process.env.PUBLISH_BINARY || false;


function build({target, runtime, abi}) {
  try {
    abi && getTarget(target, runtime);
  } catch (err) {
    return Promise.resolve();
  }

  const args = ['--verbose', '-r', runtime, '-t', target];

  if (libc.isNonGlibcLinux) {
    process.env.LIBC = libc.family;
  }

  if (PUBLISH_BINARY) {
    args.push('-u', PREBUILD_TOKEN);
  }

  return new Promise((resolve, reject) => {
    const proc = spawn('prebuild', args, { env: npmRunPath.env() });

    proc.stdout.pipe(process.stdout);
    proc.stderr.pipe(process.stderr);
    proc.on('exit', (code) => {
      if (code) {
        return reject(code);
      }

      resolve(null);
    });
  });
}


build({ runtime: 'node', target: process.versions.node, abi: false })
  .catch(code => process.exit(code));
