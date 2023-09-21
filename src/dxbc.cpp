#include <dxbc.hpp>
#include <fstream>
#include <sstream>

namespace dxdec {
template <class T>
static T ReadBits(uint32_t token, uint8_t pos, uint8_t n_bits) {
  return (token >> pos) & ((1 << n_bits) - 1);
}

template <class T> static RegisterType GetRegisterType(const T &p) {
  return static_cast<RegisterType>(p.reg_type_1 | p.reg_type_2 << 3);
}

static bool ParseInstruction(DXBC &bc, uint32_t token, uint32_t *&token_stream,
                             size_t &data_size) {
  Instruction instr;
  instr.code = static_cast<OpCode>(ReadBits<uint16_t>(token, 0, 16));
  RegisterType reg;
  switch (instr.code) {
  case OpCode::NOP:
    break;

  // 1 dst, 0 source
  case OpCode::TEX:
    instr.destination.emplace_back(*(DestinationParameter *)token_stream++);
    data_size -= 4;

    // For PS 1_4+ there are additional operands
    if (bc.major_version > 2 || bc.minor_version >= 4) {
      instr.source.emplace_back(*(SourceParameter *)token_stream++);
      data_size -= 4;
      instr.source.emplace_back(*(SourceParameter *)token_stream++);
      data_size -= 4;
    }
    break;

  // 1 dst, 1 source
  case OpCode::MOV:
  case OpCode::RCP:
  case OpCode::RSQ:
    instr.destination.emplace_back(*(DestinationParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    break;
  // 1 dst, 2 source
  case OpCode::ADD:
  case OpCode::SUB:
  case OpCode::MUL:
  case OpCode::CRS:
  case OpCode::DP3:
  case OpCode::DP4:
  case OpCode::MIN:
  case OpCode::MAX:
  case OpCode::M4x4:
  case OpCode::M4x3:
    instr.destination.emplace_back(*(DestinationParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    break;

  // 1 dst, 3 source
  case OpCode::MAD:
    instr.destination.emplace_back(*(DestinationParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    instr.source.emplace_back(*(SourceParameter *)token_stream++);
    data_size -= 4;
    break;

  default:
    return false;
  }

  bc.Instructions.emplace_back(instr);
  return true;
}
} // namespace dxdec

// See
// https://learn.microsoft.com/en-us/windows-hardware/drivers/display/shader-code-tokens
dxdec::Error dxdec::LoadBytecodeFromMemory(uint8_t *data, size_t data_size,
                                           DXBC &result) {
  // We need a valid header atleast
  if (data_size < 4)
    return Error::INVALID_HEADER;

  uint32_t *token_stream = (uint32_t *)data;
  // Retrieve the version token
  uint32_t version_token = *token_stream++;
  result.minor_version = ReadBits<uint8_t>(version_token, 0, 8);
  result.major_version = ReadBits<uint8_t>(version_token, 8, 8);
  uint16_t shader_type = ReadBits<uint16_t>(version_token, 16, 16);
  if (shader_type == 0xFFFF)
    result.type = ShaderType::PIXEL;
  else if (shader_type == 0xFFFE)
    result.type = ShaderType::VERTEX;
  else
    return Error::INVALID_HEADER;

  while (data_size) {
    uint32_t token = *token_stream++;

    // End token
    if (token == 0x0000FFFF) {
      break;
    }

    // Check if instruction or comment
    uint8_t lastbit = ReadBits<uint8_t>(token, 31, 1);

    // Only valid for instructions
    if (lastbit == 0) {
      // Comment token
      if (token & 0xFFFF == 0xFFFE)
        return Error::UNSUPPORTED;

      if (!ParseInstruction(result, token, token_stream, data_size))
        return Error::UNKNOWN_INSTRUCTION;
    } else {
      return Error::INVALID_TAG;
    }
  }

  return Error::NO_ERROR;
}

dxdec::Error dxdec::LoadBytecodeFromFile(const char *filepath, DXBC &result) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  if (!file.read((char *)buffer.data(), size)) {
    return Error::FILE_FAIL;
  }

  return LoadBytecodeFromMemory(buffer.data(), buffer.size(), result);
}

namespace dxdec {
static const char *GetOpName(OpCode op) {
  switch (op) {
  case OpCode::NOP:
    return "nop";
  case OpCode::MOV:
    return "mov";
  case OpCode::ADD:
    return "add";
  case OpCode::SUB:
    return "sub";
  case OpCode::MAD:
    return "mad";
  case OpCode::MUL:
    return "mul";
  case OpCode::RCP:
    return "rcp";
  case OpCode::RSQ:
    return "rsq";
  case OpCode::DP3:
    return "dp3";
  case OpCode::DP4:
    return "dp4";
  case OpCode::MIN:
    return "min";
  case OpCode::MAX:
    return "max";
  case OpCode::SLT:
    return "slt";
  case OpCode::SGE:
    return "sge";
  case OpCode::EXP:
    return "exp";
  case OpCode::LOG:
    return "log";
  case OpCode::LIT:
    return "lit";
  case OpCode::DST:
    return "dst";
  case OpCode::LRP:
    return "lrp";
  case OpCode::FRC:
    return "frc";
  case OpCode::M4x4:
    return "m4x4";
  case OpCode::M4x3:
    return "m4x3";
  case OpCode::M3x4:
    return "m3x4";
  case OpCode::M3x3:
    return "m3x3";
  case OpCode::M3x2:
    return "m3x2";
  case OpCode::CALL:
    return "call";
  case OpCode::CALLNZ:
    return "callnz";
  case OpCode::LOOP:
    return "loop";
  case OpCode::RET:
    return "ret";
  case OpCode::ENDLOOP:
    return "endloop";
  case OpCode::LABEL:
    return "label";
  case OpCode::DCL:
    return "dcl";
  case OpCode::POW:
    return "pow";
  case OpCode::CRS:
    return "crs";
  case OpCode::SGN:
    return "sgn";
  case OpCode::ABS:
    return "abs";
  case OpCode::NRM:
    return "nrm";
  case OpCode::TEX:
    return "tex";
  case OpCode::SINCOS:
  case OpCode::REP:
  case OpCode::ENDREP:
  case OpCode::IF:
  case OpCode::IFC:
  case OpCode::ELSE:
  case OpCode::ENDIF:
  case OpCode::BREAK:
  case OpCode::BREAKC:
  case OpCode::MOVA:
  case OpCode::DEFB:
  case OpCode::DEFI:
  case OpCode::TEXCOORD:
  case OpCode::TEXKILL:
  case OpCode::TEXBEM:
  case OpCode::TEXBEML:
  case OpCode::TEXREG2AR:
  case OpCode::TEXREG2GB:
  case OpCode::TEXM3x2PAD:
  case OpCode::TEXM3x2TEX:
  case OpCode::TEXM3x3PAD:
  case OpCode::TEXM3x3TEX:
  case OpCode::RESERVED0:
  case OpCode::TEXM3x3SPEC:
  case OpCode::TEXM3x3VSPEC:
  case OpCode::EXPP:
  case OpCode::LOGP:
  case OpCode::CND:
  case OpCode::DEF:
  case OpCode::TEXREG2RGB:
  case OpCode::TEXDP3TEX:
  case OpCode::TEXM3x2DEPTH:
  case OpCode::TEXDP3:
  case OpCode::TEXM3x3:
  case OpCode::TEXDEPTH:
  case OpCode::CMP:
  case OpCode::BEM:
  case OpCode::DP2ADD:
  case OpCode::DSX:
  case OpCode::DSY:
  case OpCode::TEXLDD:
  case OpCode::SETP:
  case OpCode::TEXLDL:
  case OpCode::BREAKP:
  case OpCode::PHASE:
  case OpCode::COMMENT:
  case OpCode::END:
  case OpCode::FORCE_DWORD:
  default:
    break;
  }
  return nullptr;
}

static const char *GetOpSuffix(Instruction i) {
  if (i.destination.size() == 1) {
    switch (i.destination[0].shift_scale) {
    case 1:
      return "_x2";
    case 2:
      return "_x4";
    case 3:
      return "_x8";
    default:
      break;
    }
  }

  return "";
}

static std::string GetRegisterName(RegisterType type, int num,
                                   bool is_ps = false) {
  std::string name;
  switch (type) {
  case RegisterType::TEMP:
    name = "r";
    name += std::to_string(num);
    break;
  case RegisterType::INPUT:
    name = "v";
    name += std::to_string(num);
    break;
  case RegisterType::ATTROUT:
    name = "oD";
    name += std::to_string(num);
    break;
  // Those are the same
  // case RegisterType::ADDR:
  case RegisterType::TEXTURE:
    name = is_ps ? "t" : "a";
    name += std::to_string(num);
    break;
  case RegisterType::CONST:
    name = "c";
    name += std::to_string(num);
    break;
  case RegisterType::RASTOUT:
    name = "oPos";
    break;
  case RegisterType::TEXCRDOUT:
    name = "oT";
    name += std::to_string(num);
    break;
  default:
    name = "UNKN";
    name += std::to_string(num);
    break;
  }

  return name;
}

static std::string GetDestinationMask(DestinationParameter p) {
  // no mask
  if (p.x_write == 1 && p.y_write == 1 && p.z_write == 1 && p.w_write == 1)
    return "";

  std::string mask = ".";
  if (p.x_write)
    mask += 'x';
  if (p.y_write)
    mask += 'y';
  if (p.z_write)
    mask += 'z';
  if (p.w_write)
    mask += 'w';

  return mask;
}

static const char *GetSourceModifier(SourceParameter p) {
  switch (p.source_modifier) {
  case SourceModifier::NONE:
    return "";
  case SourceModifier::NEGATE:
    return "-";
  default:
    return nullptr;
  }
}

static std::string GetSourceSwizzle(SourceParameter p) {
  // no swizzle
  if (p.x_swizzle == 0 && p.y_swizzle == 1 && p.z_swizzle == 2 &&
      p.w_swizzle == 3)
    return "";

  auto swizzle_mapping = [](uint8_t sw) {
    if (sw == 0)
      return 'x';
    if (sw == 1)
      return 'y';
    if (sw == 2)
      return 'z';
    if (sw == 3)
      return 'w';
    return '\0';
  };

  // Use character arithmetic to calculate the swizzle string
  std::string swizzle = ".";
  swizzle += swizzle_mapping(p.x_swizzle);
  swizzle += swizzle_mapping(p.y_swizzle);
  swizzle += swizzle_mapping(p.z_swizzle);
  swizzle += swizzle_mapping(p.w_swizzle);

  return swizzle;
}

} // namespace dxdec

bool dxdec::DisassembleBytecode(const DXBC &input, std::string &output) {
  std::stringstream ss;
  if (input.type == ShaderType::VERTEX)
    ss << "vs." << input.major_version << "." << input.minor_version << "\n";
  else if (input.type == ShaderType::PIXEL)
    ss << "ps." << input.major_version << "." << input.minor_version << "\n";
  else
    return false;

  bool is_ps = input.type == ShaderType::PIXEL;
  for (auto &instr : input.Instructions) {
    ss << GetOpName(instr.code) << GetOpSuffix(instr);

    for (const auto &dst : instr.destination) {
      auto reg_type = GetRegisterType(dst);
      ss << " " << GetRegisterName(reg_type, dst.reg_num, is_ps)
         << GetDestinationMask(dst);
    }

    for (const auto &src : instr.source) {
      auto reg_type = GetRegisterType(src);
      if (src.rel_addr) {
        ss << ", " << GetSourceModifier(src)
           << GetRegisterName(reg_type, src.reg_num) << "[a0.x]";
      } else {
        ss << ", " << GetSourceModifier(src)
           << GetRegisterName(reg_type, src.reg_num, is_ps)
           << GetSourceSwizzle(src);
      }
    }
    ss << "\n";
  }

  output = ss.str();
  return true;
}