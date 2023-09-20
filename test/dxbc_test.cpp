#include <dxbc.hpp>

#include <cstdlib>
#include <iostream>

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