#pragma once

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
		stopped_ = false;
		// The following approach has the unintended consequence
		// that the period is always increased by the execution
		// time of the callback and the following lambda. The
		// best would be relegate the callable call to another
		// detached thread so that we can immediatally enter
		// sleep_for again. On the other hand it might be too
		// thread-intense. To investigate.
		std::thread thr([=]() {
			while (!this->stopped_) {
				std::this_thread::sleep_for(period);
				if (this->stopped_) {
					// Just stop in case the task was canceled
					// during the wait.
					return;
				}
				callable();
			}
		});
		thr.detach();
	}

	void stop() {
		// We cannot stop the thread itself because we don't have
		// any reference to it. Instead, we will let it know that
		// it must not perform the requested action.
		stopped_ = true;
	}

private:
	// The stop flag does not have to be an atomic object to protect
	// it from data races. The only possible conflict could happen
	// when the timer is stopped right after starting it.
	bool stopped_ = true;
};