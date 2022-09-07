## C++ recruitment task

> Implement a class that provides the general usage
> one-shot or a periodic timer with the possibility
> to pass a callback that will be executed when
> the timer period expires.
>
> API should provide a way to schedule the following callbacks:
>  - Timer starting after the defined delay.
>  - Timer with the possibility to schedule an action with
>    the defined period.
>
> API should provide a way to stop the timer. Implement
> possible use cases with a timer written by you. Create
> unit tests for your implementation.
>
> Implementationand API design is up to you, but please
> use C++ standard library.
>
> Please provide the solution as a Visual Studio project.

### Solution description

As requested, I provided a single *timer* data type with two exclusive
modes of operation: a delayed action and a periodic action. I assumed
that the timer must not block the main execution thread. The timer can
be restarted in a different mode, with a different callback and period.
The only restriction is that one timer can manage one callback at a time.

The timer will join the worker thread on its destruction. This
is not ideal, since every time the timer goes out of scope, the calling
thread would be blocked. I would suggest to store timer instances in some
sort of extrernal heap-allocated object.

This is not an optimal timer class in terms of performance either. The major
problem is that every timer sets up its own thread and this could lead
to too many threads running on the system. If multiple timers are needed,
some sort of an event-loop would be a better solution.

This solution has two subprojects:
 * MyTimer - the timer implementation and a console app with the timer
   usage examples.
 * MyTimerTest - a separate container for the timer unit tests implemented
   with Google Test.

### Other considered approaches

 * Use `thread::detach` to dispatch the callbacks.
    - It might crash once the main thread finishes but there are
      still some detached threads running.
 * Create a pure interafce for the timer (`setDelay`, `setCallback`,
   `stop`, `start`) and implement two distinct classes over this
   interface, one for periodical calls, and one for single calls.
    - Dismissed, because the spec specifically request a single
      type with two functionalities: delayed and periodical call.
    - Also, **YAGNI**.
 * Make the call methods take explicit argument types of
   `std::function` and `std::chrono::duration`.
    - The benefit would be that the methods could be implemented
      inside a .cpp file, instead of doing it in the header.
    - On the other hand, the methods seem to be small enough
      to leave them in the header.

## Homework

Here are my answers to the homework suggested during the interiew.

### What is a semaphore and how is it different from a mutex?

As per the [`std::counting_semaphore`](https://en.cppreference.com/w/cpp/thread/counting_semaphore)
specification:

> A counting_semaphore is a lightweight synchronization primitive
> that can control access to a shared resource. Unlike a std::mutex,
> a counting_semaphore allows more than one concurrent access to the
> same resource, for at least LeastMaxValue concurrent accessors.

Interestingly, the standard semaphore has a specialization for
the max counter equal to one: `std::binary_semaphore`. Based only
off this excerpt from the specification, it could be argued that this
specialization is semantically identical to the mutex, but this is not
the case. The mutex is specifically designed to physicially stop a thread
before a critial section, while the semaphore is more of a signaling
mechanism that can be used to communicate between threads, but it does
not physically prevent other threads from accessing a resource.

The standard semaphore was introduced in `c++20`.

### How an atomic operation is defined?

An operation is considered atomic if it can be guaranteed that
an access to the value it operates on will complete without
an interruption from another thread which could result in
UB - in practise a torn read or write.

An atomic operation can usually be only a single read or write;
the standard `std::atomic` type allows also for atomic
read-modify-write operations with the means of special member
functions. I would expect that, depending on the implementation,
the compiler might guarantee no UB without any synchronization
primitives by the means of clever data alignment for small integral
or POD types.

### What is a difference between virtual and regular inheritance?

The key difference is that the virtual inheritance deduplicates
the base class in the representation of the most specialized class
in the diamond inheritance problem, unlike the regular inheritance.
If the regular inheritance is used for implementing a diamond like class
hierarchy, then the classes of the most specialized class would have
the most generic object twice in its representation, with
the intermediate classes using their separate instrances of the same
base type with potentially different states.

In general, using regular inheritance in a diamond-like class
structure is a design mistake. It is important to note that
the virtual inheritance incurs a runtime penalty (the cost of
vtable access), so it should be used only if the regular inheritance
is not sufficient to properly model planned relations.

Here is a CppCon talk which tackles the diamond problem and shows
how the virtual inheritance helps to combat it:

[![CppCon 2017: Arthur O'Dwyer "dynamic_cast From Scratch"](https://img.youtube.com/vi/QzJL-8WbpuU/0.jpg)](https://www.youtube.com/watch?v=QzJL-8WbpuU)