#include <dxbc.hpp>

#include <cstdlib>

int main(int argc, char **argv) {
  if (argc != 2)
    return EXIT_FAILURE;

  dxdec::DXBC bytecode;
  if (!dxdec::LoadBytecodeFromFile(argv[1], bytecode))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}