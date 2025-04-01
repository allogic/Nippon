#include <np_core_pch.hpp>
#include <np_core_file_util.hpp>

void NpFileUtil::ReadBinary(std::filesystem::path const &File, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream(File, std::ios::ate | std::ios::binary);
  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());
    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFileUtil::ReadText(std::filesystem::path const &File, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream(File, std::ios::ate);
  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());
    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}

void NpFileUtil::WriteBinary(std::filesystem::path const &File, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream(File, std::ios::binary);
  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFileUtil::WriteBinary(std::filesystem::path const &File, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream(File, std::ios::binary);
  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}

void NpFileUtil::WriteText(std::filesystem::path const &File, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream(File);
  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFileUtil::WriteText(std::filesystem::path const &File, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream(File);
  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}
