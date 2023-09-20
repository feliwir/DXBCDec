#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace dxdec {
enum class ShaderType {
  UNDEFINED,
  VERTEX,
  PIXEL,
};

enum class Error {
  NO_ERROR,
  FILE_FAIL,
  INVALID_HEADER,
  INVALID_TAG,
  UNKNOWN_VERSION,
  UNKNOWN_INSTRUCTION,
  UNSUPPORTED
};

enum class OpCode : uint16_t {
  NOP,
  MOV,
  ADD,
  SUB,
  MAD,
  MUL,
  RCP,
  RSQ,
  DP3,
  DP4,
  MIN,
  MAX,
  SLT,
  SGE,
  EXP,
  LOG,
  LIT,
  DST,
  LRP,
  FRC,
  M4x4,
  M4x3,
  M3x4,
  M3x3,
  M3x2,
  CALL,
  CALLNZ,
  LOOP,
  RET,
  ENDLOOP,
  LABEL,
  DCL,
  POW,
  CRS,
  SGN,
  ABS,
  NRM,
  SINCOS,
  REP,
  ENDREP,
  IF,
  IFC,
  ELSE,
  ENDIF,
  BREAK,
  BREAKC,
  MOVA,
  DEFB,
  DEFI,
  TEXCOORD = 64,
  TEXKILL,
  TEX,
  TEXBEM,
  TEXBEML,
  TEXREG2AR,
  TEXREG2GB,
  TEXM3x2PAD,
  TEXM3x2TEX,
  TEXM3x3PAD,
  TEXM3x3TEX,
  RESERVED0,
  TEXM3x3SPEC,
  TEXM3x3VSPEC,
  EXPP,
  LOGP,
  CND,
  DEF,
  TEXREG2RGB,
  TEXDP3TEX,
  TEXM3x2DEPTH,
  TEXDP3,
  TEXM3x3,
  TEXDEPTH,
  CMP,
  BEM,
  DP2ADD,
  DSX,
  DSY,
  TEXLDD,
  SETP,
  TEXLDL,
  BREAKP,
  PHASE,
  COMMENT,
  END,
  FORCE_DWORD
};

enum class RegisterType {
  TEMP,
  INPUT,
  CONST,
  ADDR = 3,    // Only VS
  TEXTURE = 3, // Only PS
  RASTOUT,
  ATTROUT,
  TEXCRDOUT,
  OUTPUT,
  CONSTINT,
  COLOROUT,
  DEPTHOUT,
  SAMPLER,
  CONST2,
  CONST3,
  CONST4,
  CONSTBOOL,
  LOOP,
  TEMPFLOAT16,
  MISCTYPE,
  LABEL,
  PREDICATE,
  FORCE_DWORD
};

enum class SourceModifier : uint8_t {
  NONE,
  NEGATE,
  BIAS,
  BIAS_AND_NEGATE,
  SIGN,
  SIGN_AND_NEGATE,
  COMPLEMENT,
  X2,            // PS 1_4 upwards
  X2_AND_NEGATE, // PS 1_4 upwards
  DIVIDE_BY_Z,   // PS 1_4 upwards
  DIVIDE_BY_W,   // PS 1_4 upwards
  ABS,
  ABS_AND_NEGATE,
  NOT
};

struct SourceParameter {
  uint16_t reg_num : 11;
  uint8_t reg_type_2 : 2;
  uint8_t rel_addr : 1;
  uint8_t reserved : 2;
  uint8_t x_swizzle : 2;
  uint8_t y_swizzle : 2;
  uint8_t z_swizzle : 2;
  uint8_t w_swizzle : 2;
  SourceModifier source_modifier : 4;
  uint8_t reg_type_1 : 3;
  uint8_t lastbit : 1;
};

static_assert(sizeof(SourceParameter) == sizeof(uint32_t),
              "SourceParameter struct compiles with wrong size");

struct DestinationParameter {
  uint16_t reg_num : 11;
  uint8_t reg_type_2 : 2;
  uint8_t rel_addr : 1;
  uint8_t reserved : 2;
  uint8_t x_write : 1;
  uint8_t y_write : 1;
  uint8_t z_write : 1;
  uint8_t w_write : 1;
  uint8_t result_modifier : 4;
  uint8_t shift_scale : 4;
  uint8_t reg_type_1 : 3;
  uint8_t lastbit : 1;
};

static_assert(sizeof(DestinationParameter) == sizeof(uint32_t),
              "DestinationParameter struct compiles with wrong size");

struct Instruction {
  OpCode code;
  std::vector<SourceParameter> source;
  std::vector<DestinationParameter> destination;
};

struct DXBC {
  ShaderType type = ShaderType::UNDEFINED;
  int major_version = -1;
  int minor_version = -1;
  std::vector<Instruction> Instructions;
};

Error LoadBytecodeFromMemory(uint8_t *data, size_t data_size, DXBC &result);
Error LoadBytecodeFromFile(const char *filepath, DXBC &result);

bool DisassembleBytecode(const DXBC &input, std::string &output);

} // namespace dxdec