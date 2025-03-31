#include <np_core_pch.hpp>
#include <np_core_binary_processor.hpp>

NpBinaryProcessor::NpBinaryProcessor(uint8_t *Bytes, uint64_t Size) : m_Bytes(Bytes), m_Size(Size) {}
NpBinaryProcessor::NpBinaryProcessor(uint8_t const *Bytes, uint64_t Size) : m_Bytes((uint8_t *)Bytes), m_Size(Size) {}
NpBinaryProcessor::~NpBinaryProcessor() {}

std::vector<uint8_t> NpBinaryProcessor::ReadByteRange(uint64_t Size) {
  std::vector<uint8_t> result = {};
  result.resize(Size);
  std::memcpy(result.data(), m_Bytes + m_Position, Size);
  m_Position += Size;
  return result;
}
std::string NpBinaryProcessor::ReadStringRange(uint64_t Size) {
  std::string result = "";
  result.resize(Size);
  std::memcpy(result.data(), m_Bytes + m_Position, Size);
  m_Position += Size;
  return result;
}

void NpBinaryProcessor::WriteByteRange(uint8_t const *Bytes, uint64_t Size) {
  std::memcpy(m_Bytes + m_Position, Bytes, Size);
  m_Position += Size;
}
void NpBinaryProcessor::WriteStringRange(char const *Bytes, uint64_t Size) {
  std::memcpy(m_Bytes + m_Position, Bytes, Size);
  m_Position += Size;
}
