#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <memory>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/weighted_sum.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_median.hpp>
#include <boost/accumulators/statistics/weighted_tail_quantile.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


#include <v8.h>
#include <node_object_wrap.h>

using namespace boost::accumulators;

namespace accumulator
{
	typedef accumulator_set<double, stats<tag::weighted_mean,
		tag::weighted_median,
		tag::weighted_sum,
		tag::weighted_tail_quantile<right>,
		tag::sum_of_weights,
		tag::min,
		tag::max>, double> accumulator_t;
	class AccumulatorSet : node::ObjectWrap
	{
	public:
		static void Init(v8::Local<v8::Object> exports);		
	private:
		std::shared_ptr<accumulator_t> accumulatorSet;
		
		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Accumulate(const v8::FunctionCallbackInfo<v8::Value>& args);
		static v8::Persistent<v8::Function> constructor;

		static void Mean(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Median(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Sum(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Quantile(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void SumOfWeights(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Min(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Max(const v8::FunctionCallbackInfo<v8::Value>& args);
		explicit AccumulatorSet();
		~AccumulatorSet();
	};
}

#endif
