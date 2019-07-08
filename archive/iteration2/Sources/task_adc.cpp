/*
 * adctask.cpp
 *
 *  Created on: 7 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */


#include "api_adc.h"
#include "api_timer.h"
#include "Bit1.h"

/***************************************************************************/
/* ADC task                                                                */
/***************************************************************************/

using namespace scp::core;

#define ADC_VERSION 0

#if ADC_VERSION == 0

extern volatile unsigned long __idle_count;
extern volatile unsigned long __timer_count;
extern volatile int16_t __accel_x;
extern volatile int16_t __accel_y;
extern volatile int16_t __accel_z;
extern volatile uint8_t __accel_whoami;
extern volatile int16_t __accel_count;

static int count = 0;

resumable adcTaskFn(uint8_t pin) {
	co_await suspend_always{};

	auto okx = co_await scp::drivers::start_adc(ADC_CHANNEL_X);
	auto oky = co_await scp::drivers::start_adc(ADC_CHANNEL_Y);

	for (;;) {
		auto x = co_await scp::drivers::read_adc3(ADC_CHANNEL_X);
		auto y = co_await scp::drivers::read_adc3(ADC_CHANNEL_Y);

		trace("x,y (accel) [t,i] : %d,%d (%d,%d,%d,%x,%d) [%lu,%lu]\r\n",
				x, y,
				__accel_x, __accel_y, __accel_z, __accel_whoami, __accel_count,
				__timer_count, __idle_count);
		Bit1_PutVal(++count % 2 == 0);
		co_await scp::drivers::wait_on_ticks(10);
	}
}

#endif

#if ADC_VERSION == 1

resumable adcTaskFn(uint8_t pin) {
	trace("adcTaskFn() create\r\n");
	co_await suspend_always{};
	trace("adcTaskFn() start\r\n");

	auto fcx = start_adc(ADC_CHANNEL_X);
	auto fcy = start_adc(ADC_CHANNEL_Y);
	auto okx = co_await fcx;
	auto oky = co_await fcy;

	for (;;) {
		auto fmx = read_adc(ADC_CHANNEL_X);
		auto fmy = read_adc(ADC_CHANNEL_Y);

		auto x = co_await fmx;
		auto y = co_await fmy;

		trace("x,y : %d,%d\r\n", x, y);
		//auto result = co_await transmit_data(value);
		//trace("transmit(%u)=%u\r\n", value, result);
	}
}

#endif // VERSION == 1

#if ADC_VERSION == 2

namespace scheduling {
	template <typename T1, typename T2>
	future_t<std::tuple<T1, T2>, awaitable_state<std::tuple<T1, T2>>>
		future_of_both1(future_t<T1, awaitable_state<T1>>&& f1,
									 future_t<T2, awaitable_state<T2>>&& f2)
	{
		std::tuple<T1, T2> result(
			co_await f1,
			co_await f2);
		promise_t< std::tuple<T1, T2> > p;
		//p.return_value(result);
		return p.get_future();
	}

	template <typename T1, typename T2>
	future_t< std::tuple<T1, T2> > future_of_both(future_t<T1>&& f1, future_t<T2>&& f2)
	{
		typedef std::tuple<T1, T2> result_t;
		promise_t<result_t> p;
		auto t1 = co_await f1;
		auto t2 = co_await f2;
		//result_t r(co_await f1, co_await f2);
		//p.return_value(r);
		return p.get_future();
	}
}

resumable adcTaskFn(uint8_t pin) {
	co_await suspend_always{};

	//auto ok = co_await (start_adc(ADC_CHANNEL_X) && start_adc(ADC_CHANNEL_Y));
	//co_await future_of_all(start_adc(ADC_CHANNEL_X), start_adc(ADC_CHANNEL_Y));
	std::vector<future_t<byte> > vf;
	vf.push_back(start_adc(ADC_CHANNEL_X));
	vf.push_back(start_adc(ADC_CHANNEL_Y));
	//auto ok = future_of_all_range<std::vector<future_t<byte> >::iterator>(vf.begin(), vf.end());

	for (;;) {
		//auto fmx = read_adc(ADC_CHANNEL_X);
		//auto fmy = read_adc(ADC_CHANNEL_Y);
		//co_await future_of_all(fmx, fmy);
		//co_await fmx && fmy;
		//co_await read_adc(ADC_CHANNEL_X) && read_adc(ADC_CHANNEL_Y);
		//future_of_all2(read_adc(ADC_CHANNEL_X), read_adc(ADC_CHANNEL_Y));
		auto r = future_of_both(read_adc(ADC_CHANNEL_X), read_adc(ADC_CHANNEL_Y));

		//trace("x,y = %d,%d\r\n", coords[0], coords[1]);
		//auto result = co_await transmit_data(value);
		//trace("transmit(%u)=%u\r\n", value, result);
	}
}

#endif // VERSION == 2

#if ADC_VERSION == 3

namespace scheduling {
	/*
	template <typename T1, typename T2>
	class future_of_both_t {
	public:
		future_of_both_t(future_t<T1>&& f1, future_t<T2>&& f2)
			: f1_(f1), f2_(f2)
		{
		}
		future_t<T1>&& f1_;
		future_t<T2>&& f2_;
	};
	*/
	template <typename F1, typename F2>
	class future_of_both_t {
	public:
		future_of_both_t(F1&& f1, F2&& f2)
			: f1_(std::move(f1)), f2_(std::move(f2))
		{
		}
		F1 f1_;
		F2 f2_;
		auto get_value() {
			while (!f1_.ready() || !f2_.ready())
			{
			}
			return std::make_pair(f1_.get_value(), f2_.get_value());
		}
	};

	template <typename T1, typename S1, typename T2, typename S2>
	class future_of_both_t2 {
	public:
		typedef future_t<T1, S1> f1_type;
		typedef future_t<T2, S2> f2_type;
		future_of_both_t2(f1_type&& f1, f2_type&& f2)
			: f1_(std::move(f1)), f2_(std::move(f2))
		{
		}
		auto operator co_await() {
			auto t1 = co_await f1_;
			auto t2 = co_await f2_;
			return get_value();
		}
		std::pair<T1, T2> get_value() {
			while (!f1_.ready() || !f2_.ready())
			{
			}
			return std::make_pair(f1_.get_value(), f2_.get_value());
		}
	private:
		f1_type f1_;
		f2_type f2_;
	};

	template <typename F1, typename F2>
	future_of_both_t < F1, F2 >  f_of_both(
		F1&& f1, F2&& f2)
	{
		return future_of_both_t< F1, F2 >(
			std::move(f1), std::move(f2)
			);
	}

	template <typename F1, typename F2>
	auto f_of_both2(F1&& f1, F2&& f2)
	{
		return future_of_both_t2< F1::type, F1::state_type, F2::type, F2::state_type >(
			std::forward<F1&&>(f1), std::forward<F2&&>(f2)
			);
	}
	template <typename F1, typename F2>
	auto f_of_both2(F1& f1, F2& f2)
	{
		return future_of_both_t2< F1::type, F1::state_type, F2::type, F2::state_type >(
			std::forward<F1&&>(f1), std::forward<F2&&>(f2)
			);
	}

	template<typename T1, typename T2>
	auto operator&&(future_t<T1>&& t1, future_t<T2>&& t2)
	{
		return f_of_both(t1, t2);
	}
}



resumable adcTaskFn(uint8_t pin) {
	co_await suspend_always{};

	//auto ok = co_await (start_adc(ADC_CHANNEL_X) && start_adc(ADC_CHANNEL_Y));
	//co_await future_of_all(start_adc(ADC_CHANNEL_X), start_adc(ADC_CHANNEL_Y));
	std::vector<future_t<byte> > vf;
	vf.push_back(start_adc(ADC_CHANNEL_X));
	vf.push_back(start_adc(ADC_CHANNEL_Y));
	//auto ok = future_of_all_range<std::vector<future_t<byte> >::iterator>(vf.begin(), vf.end());

	for (;;) {

		//auto b2 = scheduling::f_of_both(fmx, fmy);
/*
		auto fmx = read_adc(ADC_CHANNEL_X);
		auto fmy = read_adc(ADC_CHANNEL_Y);
		scheduling::future_of_both_t<decltype(fmx), decltype(fmy)> fb(
			std::move(fmx), std::move(fmy));
			//fmx, fmy);
		auto r = fb.get_value();
		*/
		/*
		scheduling::future_of_both_t<decltype(read_adc(0)), decltype(read_adc(0))> fb(
			//std::move(fmx), std::move(fmy));
			read_adc(ADC_CHANNEL_X), read_adc(ADC_CHANNEL_Y));
		auto r = fb.get_value();
		*/
		if (1)
		{
			auto b3 = f_of_both2(read_adc(ADC_CHANNEL_X), read_adc(ADC_CHANNEL_Y));
			//auto b3 = read_adc(ADC_CHANNEL_X) && read_adc(ADC_CHANNEL_Y);
			auto r = b3.get_value();
			trace("x,y = %d,%d\r\n", r.first, r.second);
		}
		else
		{
			auto fmx = read_adc(ADC_CHANNEL_X);
			auto fmy = read_adc(ADC_CHANNEL_Y);
			auto b3 = f_of_both2(fmx, fmy);
			auto r = b3.get_value();
			trace("x,y = %d,%d\r\n", r.first, r.second);
		}
	}
}

#endif // VERSION == 3

#if ADC_VERSION == 4

namespace scheduling {

	template <typename T, typename S = >
	class future_all_t {
	public:
		typedef future_t<T1> future_1_type;
		typedef future_t<T2> future_2_type;
		//typedef future_t
	};

	template <typename T1, typename T2>
	future_t<void> all_of(future_t<T1>&& f1, future_t<T2>&& f2) {

	}
}


resumable adcTaskFn(uint8_t pin) {
	co_await suspend_always{};
	co_await start_adc(ADC_CHANNEL_X);
	co_await start_adc(ADC_CHANNEL_Y);

	for (;;) {
		auto fmx = read_adc(ADC_CHANNEL_X);
		auto fmy = read_adc(ADC_CHANNEL_Y);

		// error C3313: 'r': variable cannot have the type 'void'
		//auto r = co_await future_of_any(fmx, fmy);

		// ok
		//co_await future_of_any(fmx, fmy);

		auto fma = future_of_any(fmx, fmy);
		co_await fma;
		//auto r = fma.get_value();
		trace("wait is over\r\n");
		//trace("x,y = %d,%d\r\n", r.first, r.second);
	}
}

#endif // VERSION == 4

#if ADC_VERSION == 5

void adcSubFn() {

	auto x = co_await read_adc(ADC_CHANNEL_X);
	auto y = co_await read_adc(ADC_CHANNEL_Y);

	trace("x,y = %d,%d\r\n", x, y);
}

resumable adcTaskFn(uint8_t pin) {
	co_await suspend_always{};

	auto okx = co_await start_adc(ADC_CHANNEL_X);
	auto oky = co_await start_adc(ADC_CHANNEL_Y);

	for (;;) {
		adcSubFn();
		//auto result = co_await transmit_data(value);
		//trace("transmit(%u)=%u\r\n", value, result);
	}
}

#endif // VERSION == 5

