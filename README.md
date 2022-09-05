## C++ recruitment task

Implement a class that provides the general usage
one-shot or a periodic timer with the possibility
to pass a callback that will be executed when
the timer period expires.

API should provide a way to schedule the following callbacks:
 - Timer starting after the defined delay.
 - Timer with the possibility to schedule an action with
   the defined period.

API should provide a way to stop the timer. Implement
possible use cases with a timer written by you. Create
unit tests for your implementation.

Implementationand API design is up to you, but please
use C++ standard library.

Please provide the solution as a Visual Studio project.

## Solution description

The timer is a takes a

Other considered approaches:
 * Create a pure interafce for a timer (setDelay, setCallback,
   stop, start) and implement two distinct classes over this
   interface, one for periodical calls, and one for single calls.
    - Dismissed, because the spec specifically request a single
      type with two functionalities: delayed and periodical call.
    - Also, YAGNI.
 * Make the call methods take explicit argument types of
   std::function and std::chrono::duration.
    - The benefit would be that the methods could be implemented
      inside a .cpp files, instead of doing it in the header.
    - On the other hand, the methods seem to be small enough
      to leave them in the header

