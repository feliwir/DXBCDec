#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace dxdec {
enum class ShaderType {
  UNDEFINED,
  VERTEX,
  PIXEL,
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
  TEXCOORD,
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

struct Parameter
{
  uint16_t reg;
  uint8_t swizzle;
};

struct Instruction {
  OpCode code;
  Parameter source;
  Parameter destination;
};

struct DXBC {
  ShaderType type = ShaderType::UNDEFINED;
  int major_version = -1;
  int minor_version = -1;
  std::vector<Instruction> Instructions;
};

bool LoadBytecodeFromMemory(uint8_t *data, size_t data_size, DXBC &result);
bool LoadBytecodeFromFile(const char *filepath, DXBC &result);

} // namespace dxdec