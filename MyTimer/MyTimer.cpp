#include "Timer.h"

#include <iostream>
#include <chrono>
#include <thread>

void exampleCall() {
	std::cout << "Hello from the timer!\n";
}

int main()
{
	using namespace std::chrono_literals;
	using std::this_thread::sleep_for;

	Timer timer1;
	std::cout << "Example 1s delay over 2s in the main thread:\n";
	timer1.callWithDelay(exampleCall, 1s);
	sleep_for(2s);
	std::cout << "Example from the main thread.\n";

	const auto myLambda = []() {
		std::cout << "Hello from the lambda!\n";
	};
	std::cout << "\nExample 1s delay in a lambda over 2s in the main thread:\n";
	Timer timer2;
	timer2.callWithDelay(myLambda, 1s);
	sleep_for(2s);
	std::cout << "Example from the main thread.\n";

	std::cout << "\nExample stop:\n";
	Timer timer3;
	timer3.callWithDelay(myLambda, 2s);
	sleep_for(1s);
	timer3.stop();
	std::cout << "Has anything happened? Waiting two seconds just to make sure.\n";
	sleep_for(2s);
	std::cout << "Still nothing, good.\n";

	Timer timer4;
	std::cout << "\nExample short delay:\n";
	timer4.callWithDelay(myLambda, 100ms);
	sleep_for(1s);
	std::cout << "Whoa, that was fast!\n";

	auto counter = 0;
	const auto countedLambda = [&counter]() {
		std::cout << "Hello from the counted lambda!\n";
		++counter;
	};
	std::cout << "\nExample periodical timer:\n";
	Timer timer5;
	timer5.callWithPeriod(countedLambda, 190ms);
	sleep_for(1s);
	timer5.stop();
	std::cout << "Done. Called the lambda " << counter << " times.\n";

	Timer timer6;
	std::cout << "\nWhat will happen if I return from main before the scheduled task is done:\n";
	timer6.callWithDelay(myLambda, 1s);

	return 0;
}