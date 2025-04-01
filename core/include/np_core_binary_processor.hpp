#if !defined(NP_CORE_BINARY_PROCESSOR_HPP)
#  define NP_CORE_BINARY_PROCESSOR_HPP

#  include <np_core_macro.hpp>

class NpBinaryProcessor {
public:
  NpBinaryProcessor(uint8_t *Bytes, uint64_t Size);
  NpBinaryProcessor(uint8_t const *Bytes, uint64_t Size);
  virtual ~NpBinaryProcessor();

  inline auto const &GetPosition() const { return m_Position; }

  inline void SeekRel(int64_t Value) { m_Position += Value; }
  inline void SeekAbs(uint64_t Value) { m_Position = Value; }
  inline void AlignUp(uint64_t Alignment) { m_Position = ALIGN_UP_BY(m_Position, Alignment); }
  inline void AlignDown(uint64_t Alignment) { m_Position = ALIGN_DOWN_BY(m_Position, Alignment); }
  inline void ModUp(uint64_t Modulus) { m_Position -= m_Position % Modulus; }
  inline void ModDown(uint64_t Modulus) { m_Position += m_Position % Modulus; }

  template <typename T>
  T Read();

  template <typename T>
  std::vector<T> Read(uint64_t Count);

  template <typename T>
  void Read(std::vector<T> &Result, uint64_t Count);

  std::vector<uint8_t> ReadByteRange(uint64_t Size);
  std::string ReadStringRange(uint64_t Size);

  template <typename T>
  void Write(T Value);

  template <typename T>
  void Write(std::vector<T> const &Values);

  void WriteByteRange(uint8_t const *Bytes, uint64_t Size);
  void WriteStringRange(char const *Bytes, uint64_t Size);

private:
  uint8_t *m_Bytes = nullptr;

  uint64_t m_Size = 0;
  uint64_t m_Position = 0;
};

template <typename T>
T NpBinaryProcessor::Read() {
  T result = *((T *)(m_Bytes + m_Position));
  m_Position += sizeof(T);
  return result;
}

template <typename T>
std::vector<T> NpBinaryProcessor::Read(uint64_t Count) {
  std::vector<T> result = {};
  result.resize(Count);
  std::memcpy(result.data(), m_Bytes + m_Position, result.size() * sizeof(T));
  m_Position += result.size() * sizeof(T);
  return result;
}

template <typename T>
void NpBinaryProcessor::Read(std::vector<T> &Result, uint64_t Count) {
  Result.resize(Count);
  std::memcpy(Result.data(), m_Bytes + m_Position, Result.size() * sizeof(T));
  m_Position += Result.size() * sizeof(T);
}

template <typename T>
void NpBinaryProcessor::Write(T Value) {
  std::memcpy(m_Bytes + m_Position, &Value, sizeof(T));
  m_Position += sizeof(T);
}

template <typename T>
void NpBinaryProcessor::Write(std::vector<T> const &Values) {
  std::memcpy(m_Bytes + m_Position, Values.data(), Values.size() * sizeof(T));
  m_Position += Values.size() * sizeof(T);
}

#endif
