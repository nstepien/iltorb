import test from 'ava';

// https://nodejs.org/api/addons.html#addons_context_aware_addons
test('addon should be context-aware', async function(t) {
  if (/^v(6|8)\./.test(process.version)) {
    t.pass();
    return;
  }

  const brotli1 = require('../');
  const buf1 = await brotli1.compress(Buffer.alloc(0));
  t.truthy(Buffer.isBuffer(buf1));

  delete require.cache[require.resolve('..')];
  delete require.cache[require.resolve('../build/bindings/iltorb.node')];

  const brotli2 = require('../');
  const buf2 = await brotli2.compress(Buffer.alloc(0));
  t.truthy(Buffer.isBuffer(buf2));
  t.truthy(Buffer.compare(buf1, buf2) === 0);
});
