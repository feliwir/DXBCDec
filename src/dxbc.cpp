#include <dxbc.hpp>
#include <fstream>

namespace dxdec {
template <class T>
static T ReadBits(uint32_t token, uint8_t pos, uint8_t n_bits) {
  return (token >> pos) & ((1 << n_bits) - 1);
}

static Instruction ParseInstruction(uint32_t token, uint32_t *&token_stream,
                                    size_t &data_size) {
  Instruction instr;
  instr.code = static_cast<OpCode>(ReadBits<uint16_t>(token, 0, 16));
  uint32_t param;
  switch (instr.code) {
  case OpCode::MOV:
    param = *token_stream++;
    data_size -= 4;
    param = *token_stream++;
    data_size -= 4;
    break;

  default:
    break;
  }

  return instr;
}
} // namespace dxdec

// See
// https://learn.microsoft.com/en-us/windows-hardware/drivers/display/shader-code-tokens
bool dxdec::LoadBytecodeFromMemory(uint8_t *data, size_t data_size,
                                   DXBC &result) {
  // We need a valid header atleast
  if (data_size < 4)
    return false;

  uint32_t *token_stream = (uint32_t *)data;
  // Retrieve the version token
  uint32_t version_token = *token_stream++;
  uint8_t minor_version = ReadBits<uint8_t>(version_token, 0, 8);
  uint8_t major_version = ReadBits<uint8_t>(version_token, 8, 8);
  uint16_t shader_type = ReadBits<uint16_t>(version_token, 16, 16);
  if (shader_type == 0xFFFF)
    result.type = ShaderType::PIXEL;
  else if (shader_type == 0xFFFE)
    result.type = ShaderType::VERTEX;
  else
    return false;

  while (data_size) {
    uint32_t token = *token_stream++;

    // End token
    if (token == 0x0000FFFF) {
      break;
    }

    // Check if instruction
    uint8_t lastbit = ReadBits<uint8_t>(token, 31, 1);

    // Only valid for instructions
    if (lastbit == 0) {
      result.Instructions.emplace_back(
          ParseInstruction(token, token_stream, data_size));
    }

    token_stream++;
    data_size -= 4;
  }

  return false;
}

bool dxdec::LoadBytecodeFromFile(const char *filepath, DXBC &result) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  if (!file.read((char *)buffer.data(), size)) {
    return false;
  }

  return LoadBytecodeFromMemory(buffer.data(), buffer.size(), result);
}