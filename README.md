# DXBCDec

A **D**irect**X** **B**yte**C**ode **Dec**ompiler

## Features

- Support **ps_1_1**, **ps_1_2**, **ps_1_3**, **ps_1_4**, **VS_1_1**

## Usage

```c++
int main(int argc, char **argv) {
  if (argc != 2)
    return EXIT_FAILURE;

  dxdec::DXBC bytecode;
  if (dxdec::LoadBytecodeFromFile(argv[1], bytecode) != dxdec::Error::NO_ERROR)
    return EXIT_FAILURE;

  std::string assembly;
  dxdec::DisassembleBytecode(bytecode, assembly);
  std::cout << assembly << std::endl;

  return EXIT_SUCCESS;
}
```