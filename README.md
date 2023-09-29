# AIUnite

The Standard for Kernel Plug-ability.
A Plugin API for Execution Engines to every Kernel Generator.

            
## Dependencies

- libboost-all-dev v1.71
- libssl-dev

## Build

- git submodule update --init --recursive
- mkdir build
- cd build
- cmake ..
- cmake --build .

## Configuration

- Override llvm-project using build flags?


## Open questions

    - How to support custom ops?
    -- Linalg-on-Tensors?
    - How to support custom types?
    -- PDL?