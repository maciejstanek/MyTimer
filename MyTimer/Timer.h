#pragma once

#include <functional>
#include <chrono>
#include <thread>

struct Timer
{
	template<typename Callable, typename Delay>
	void callWithDelay(Callable callable, Delay delay) {
		stopped_ = false;
		// We need to pass the callable by value and not by
		// reference since it will be called in another thread.
		// Stop flag is passed by reference since this is passed
		// as a pointer by default.
		std::thread thr([=]() {
			std::this_thread::sleep_for(delay);
			if (!this->stopped_) {
				callable();
				this->stopped_ = true;
			}
			});
		thr.detach();
	}

	template<typename Callable, typename Period>
	void callWithPeriod(Callable callable, Period period) {
		// TODO
	}

	void stop() {
		// We cannot stop the thread itself because we don't have
		// any reference to it. Instead, we will let it know that
		// it must not perform the requested action.
		stopped_ = true;
	}

private:
	bool stopped_ = true;

};

