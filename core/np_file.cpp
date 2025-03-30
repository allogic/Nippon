#include <np_pch.hpp>
#include <np_file.hpp>

void NpFile::ReadBinary(NpPath const &Path, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream(Path, std::ios::ate | std::ios::binary);

  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());

    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::ReadText(NpPath const &Path, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream(Path, std::ios::ate);

  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());

    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}

void NpFile::WriteBinary(NpPath const &Path, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream(Path, std::ios::binary);

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::WriteBinary(NpPath const &Path, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream(Path, std::ios::binary);

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}

void NpFile::WriteText(NpPath const &Path, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream(Path);

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::WriteText(NpPath const &Path, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream(Path);

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}
