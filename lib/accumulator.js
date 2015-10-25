var bindings = require('./bindings');

module.exports.AccumulatorSet = AccumulatorSet;

function AccumulatorSet() {
    return new bindings.AccumulatorSet();
}
