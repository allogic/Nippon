#include <np_pch.hpp>
#include <np_file.hpp>

void NpFile::ReadBinary(fs::path const &FilePath, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream = std::ifstream{FilePath, std::ios::ate | std::ios::binary};

  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());

    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::ReadText(fs::path const &FilePath, std::vector<uint8_t> &Bytes) {
  std::ifstream Stream = std::ifstream{FilePath, std::ios::ate};

  if (Stream.is_open()) {
    Bytes.resize(Stream.tellg());

    Stream.seekg(std::ios::beg);
    Stream.read((char *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}

void NpFile::WriteBinary(fs::path const &FilePath, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream = std::ofstream{FilePath, std::ios::binary};

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::WriteBinary(fs::path const &FilePath, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream = std::ofstream{FilePath, std::ios::binary};

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}

void NpFile::WriteText(fs::path const &FilePath, std::vector<uint8_t> const &Bytes) {
  std::ofstream Stream = std::ofstream{FilePath};

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes.data(), Bytes.size());
    Stream.close();
  }
}
void NpFile::WriteText(fs::path const &FilePath, uint8_t const *Bytes, uint64_t Size) {
  std::ofstream Stream = std::ofstream{FilePath};

  if (Stream.is_open()) {
    Stream.write((char const *)Bytes, Size);
    Stream.close();
  }
}

void NpFile::CreateDirIfNotExist(fs::path const &FilePath, bool DotIsDirectory) {
  fs::path PartialFilePath = "";

  for (auto const &FilePathSection : FilePath) {
    PartialFilePath /= FilePathSection;

    if (!DotIsDirectory) {
      if (filePath.string().contains('.')) {
        break;
      }
    }

    if (!fs::exists(PartialFilePath)) {
      fs::create_directory(PartialFilePath);
    }
  }
}

std::vector<uint64_t> NpFile::FindStringsInFile(std::vector<uint8_t> const &Bytes, std::vector<uint8_t> const &Pattern) {
  std::vector<uint64_t> Indices = {};

  for (uint64_t ByteIndex = 0; ByteIndex < Bytes.size(); ByteIndex++) {
    bool Found = true;

    for (uint64_t PatternIndex = 0; PatternIndex < Pattern.size(); PatternIndex++) {
      if (Bytes[ByteIndex + PatternIndex] != Pattern[PatternIndex]) {
        Found = false;

        break;
      }
    }

    if (Found) {
      Indices.emplace_back(i);
    }
  }

  return Indices;
}
