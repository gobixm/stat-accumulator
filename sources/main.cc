#include <iostream>
#include <node.h>
#include "accumulator.h"

namespace accumulator
{
	using v8::Local;
	using v8::Object;

	void InitAll(Local<Object> exports) {
		AccumulatorSet::Init(exports);
	}

	NODE_MODULE(addon, InitAll)	
}
namespace boost
{
	void throw_exception(std::exception const & e)
	{
		
	}
}