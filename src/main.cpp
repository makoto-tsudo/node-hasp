#include <node.h>
#include "node_hasp.h"

using namespace v8;

NODE_MODULE_INIT(/* exports, module, context */) {
    NodeHasp::Init(exports, context);
}
