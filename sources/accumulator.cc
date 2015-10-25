#include "accumulator.h"
#include <node.h>

namespace accumulator
{
	v8::Persistent<v8::Function> AccumulatorSet::constructor;

	void AccumulatorSet::New(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		if (args.IsConstructCall())
		{
			// Invoked as constructor
			AccumulatorSet* accumulator = new AccumulatorSet();
			accumulator->Wrap(args.This());
			args.GetReturnValue().Set(args.This());
		}
		else
		{
			// Invoked as plain function
			v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
			args.GetReturnValue().Set(cons->NewInstance(0, {}));
		}
	}

	void AccumulatorSet::Accumulate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());

		if (args.Length() == 0)
		{
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}
		auto accumulatorSet = accumulator->accumulatorSet.get();
		if (args[0]->IsArray())
		{
			v8::Handle<v8::Array> numbers = v8::Handle<v8::Array>::Cast(args[0]);
			v8::Handle<v8::Array> weights;
			bool hasWeights = false;
			if (args.Length() > 1)
			{				
				if(!args[1]->IsArray())
				{
					isolate->ThrowException(v8::Exception::TypeError(
						v8::String::NewFromUtf8(isolate, "Weights must be array in case of Values are array")));
					return;
				}
				weights = v8::Handle<v8::Array>::Cast(args[1]);
				if (numbers->Length() != weights->Length())
				{
					isolate->ThrowException(v8::Exception::TypeError(
						v8::String::NewFromUtf8(isolate, "Values and Weights arrays should have same length")));
					return;
				}	
				hasWeights = true;							
			}
			for (int i = 0; i < numbers->Length(); i++)
			{
				double w = hasWeights ? weights->Get(i)->NumberValue() : 1.0;					
				accumulatorSet->operator()(numbers->Get(i)->NumberValue(), weight = w);				
			}
		}
		else
		{
			double w = args.Length()>1 ? args[1]->NumberValue() : 1.0;
			accumulatorSet->operator()(args[0]->NumberValue(), weight = w);
		}
		args.GetReturnValue().Set(sum(*accumulatorSet));
	}

	void AccumulatorSet::Reset(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		accumulator->accumulatorSet.reset(new accumulator_t(right_tail_cache_size = 20000));
	}

	void AccumulatorSet::Median(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = weighted_median(*accumulator->accumulatorSet);
		v8::Local<v8::Number> median = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(median);
	}

	void AccumulatorSet::Sum(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = weighted_sum(*accumulator->accumulatorSet);
		v8::Local<v8::Number> sum = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(sum);
	}

	void AccumulatorSet::Quantile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());		

		auto a = weighted_tail_quantile(*accumulator->accumulatorSet, quantile_probability = args[0]->NumberValue());
		v8::Local<v8::Number> quantile = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(quantile);
		
	}

	void AccumulatorSet::SumOfWeights(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = sum_of_weights(*accumulator->accumulatorSet);
		v8::Local<v8::Number> sum = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(sum);
	}

	void AccumulatorSet::Min(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = min(*accumulator->accumulatorSet);
		v8::Local<v8::Number> min = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(min);
	}

	void AccumulatorSet::Max(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = max(*accumulator->accumulatorSet);
		v8::Local<v8::Number> max = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(max);
	}

	void AccumulatorSet::Mean(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();

		AccumulatorSet* accumulator = Unwrap<AccumulatorSet>(args.Holder());
		auto a = mean(*accumulator->accumulatorSet);
		v8::Local<v8::Number> mean = v8::Number::New(isolate, a);

		args.GetReturnValue().Set(mean);
	}

	AccumulatorSet::AccumulatorSet()
	{		
		accumulatorSet = std::make_shared<accumulator_t>(right_tail_cache_size = 20000);
	}

	AccumulatorSet::~AccumulatorSet()
	{
	}

	void AccumulatorSet::Init(v8::Local<v8::Object> exports)
	{
		v8::Isolate* isolate = exports->GetIsolate();

		// Prepare constructor template
		v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
		tpl->SetClassName(v8::String::NewFromUtf8(isolate, "AccumulatorSet"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
		NODE_SET_PROTOTYPE_METHOD(tpl, "accumulate", Accumulate);
		NODE_SET_PROTOTYPE_METHOD(tpl, "mean", Mean);
		NODE_SET_PROTOTYPE_METHOD(tpl, "median", Median);
		NODE_SET_PROTOTYPE_METHOD(tpl, "sum", Sum);
		NODE_SET_PROTOTYPE_METHOD(tpl, "quantile", Quantile);
		NODE_SET_PROTOTYPE_METHOD(tpl, "sumOfWeights", SumOfWeights);
		NODE_SET_PROTOTYPE_METHOD(tpl, "min", Min);
		NODE_SET_PROTOTYPE_METHOD(tpl, "max", Max);
		NODE_SET_PROTOTYPE_METHOD(tpl, "reset", Reset);

		constructor.Reset(isolate, tpl->GetFunction());
		exports->Set(v8::String::NewFromUtf8(isolate, "AccumulatorSet"),
		             tpl->GetFunction());
	}
}
