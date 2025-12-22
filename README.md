EBus
======
Building status: [![cpp-ci-build](https://github.com/xeechou/ebus/actions/workflows/cpp.yaml/badge.svg?branch=main)](https://github.com/xeechou/ebus/actions/workflows/cpp.yaml)


EBus is a collection of event and bus hooking library to facilitate event handling in C++

- EBus event : which are type based, you can call `ebus::event()` to dispatch events.
- object based events : Which you need to call `ev.dispatch(args...)` to dispatch events.
- task scheduler : async task scheduling that allows you to chain one task after another.
- hooks : hooks system allows you to register hooks to be run later.


