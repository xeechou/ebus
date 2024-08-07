EBus
======

EBus is a collection of event and bus hooking library to facilitate event handling in C++, there are two types of events.

- EBus event which are type based, you can call `ebus::event()` to dispatch events.
- object based events. Which you need to call `ev.dispatch(args...)` to dispatch events.

## License
EBus is licensed under LGPL version 2.1. You can find the license text in
[COPYING.LESSER](COPYING.LESSER). External dependencies in the `external`
folder have their own licenses specified either at the start of each file or as
separate license text files.
