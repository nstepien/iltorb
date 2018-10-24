import test from 'ava';

test('compress an empty buffer', async function(t) {
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
