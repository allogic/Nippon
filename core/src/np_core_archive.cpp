#include <np_core_pch.hpp>
#include <np_core_archive.hpp>
#include <np_core_file_utils.hpp>
#include <np_core_path_utils.hpp>
#include <np_core_string_utils.hpp>

#define NP_ARCHIVE_FILE_HEADER_SIZE 0x20
#define NP_ARCHIVE_FILE_ALIGNMENT 0x100

#define NP_ARCHIVE_PRINT(FMT, ...)                    \
  snprintf(Buffer, sizeof(Buffer), FMT, __VA_ARGS__); \
  Callback(Buffer)

NpArchive::NpArchive() {}
NpArchive::NpArchive(NpArchive *Parent) {}
NpArchive::~NpArchive() {}

void NpArchive::SetData(uint8_t const *Bytes, uint64_t Size) {
  if (m_Bytes) {
    delete[] m_Bytes;
  }

  m_Bytes = new uint8_t[Size];
  m_SizePrev = m_Size;
  m_Size = Size;

  std::memcpy(m_Bytes, Bytes, Size);
}

void NpArchive::Serialize(std::vector<uint8_t> &Bytes) {
  UpdateSizesRecursive();
  UpdateByteArraysRecursive();

  SerializeRecursive();

  Bytes = {m_Bytes, m_Bytes + m_Size};
}
void NpArchive::Deserialize(uint8_t const *Bytes, uint64_t Size) {
  if (m_Bytes) {
    delete[] m_Bytes;
  }

  m_Bytes = new uint8_t[Size];
  m_SizePrev = m_Size;
  m_Size = Size;

  std::memcpy(m_Bytes, Bytes, Size);

  DeserializeRecursive();

  UpdateSizesRecursive();
  UpdateByteArraysRecursive();
}
void NpArchive::Deserialize(std::vector<uint8_t> const &Bytes) {
  if (m_Bytes) {
    delete[] m_Bytes;
  }

  m_Bytes = new uint8_t[](Bytes.size());
  m_SizePrev = m_Size;
  m_Size = Bytes.size();

  std::memcpy(m_Bytes, Bytes.data(), Bytes.size());

  DeserializeRecursive();

  UpdateSizesRecursive();
  UpdateByteArraysRecursive();
}

void NpArchive::ExtractToDisk(std::filesystem::path const &File) {
  NpPathUtils::CreateDir(File);

  ExtractToDiskRecursive(File);
}
void NpArchive::UnfoldToDisk(std::filesystem::path const &File) {
  NpPathUtils::CreateDir(File);

  UnfoldToDiskRecursive(File);
}
void NpArchive::PrintTableOfContent(std::function<void(char const *)> Callback, uint32_t Offset, uint32_t Indent, uint32_t Increment) {
  PrintTableOfContentRecursive(Callback, Offset, Indent, Increment);
}
void NpArchive::PrintOfType(std::function<void(char const *)> Callback, std::string const &Type) {
  PrintOfTypeRecursive(Callback, Type);
}
NpArchive *NpArchive::FindArchiveByType(std::string const &Type) {
  NpArchive *Archive = nullptr;

  FindArchiveByTypeRecursive(Type, &Archive);

  return Archive;
}
NpArchive *NpArchive::FindArchiveByName(std::string const &Name) {
  NpArchive *Archive = nullptr;

  FindArchiveByNameRecursive(Name, &Archive);

  return Archive;
}
NpArchive *NpArchive::FindArchiveByTypeAndName(std::string const &Type, std::string const &Name) {
  NpArchive *Archive = nullptr;

  FindArchiveByTypeAndNameRecursive(Type, Name, &Archive);

  return Archive;
}
std::vector<NpArchive *> NpArchive::FindArchivesByType(std::string const &Type) {
  std::vector<NpArchive *> Archives = {};

  FindArchivesByTypeRecursive(Type, Archives);

  return Archives;
}
std::vector<NpArchive *> NpArchive::FindArchivesByName(std::string const &Name) {
  std::vector<NpArchive *> Archives = {};

  FindArchivesByNameRecursive(Name, Archives);

  return Archives;
}

void NpArchive::SerializeRecursive() {
  NpBinaryProcessor Processor(m_Bytes, m_Size);

  if (m_IsDirectory) {
    WriteDirectoryHeader(&Processor);

    for (auto const &Child : m_Children) {
      Child->SerializeRecursive();

      WriteFileContent(&Processor, Child);
    }
  } else {
    if (std::strcmp(m_Type, "ROF") == 0) {
      Processor.WriteStringRange("RUNOFS64", 8);

      uint64_t Accumulator = ALIGN_UP_BY(4 + ((m_Parent->m_Children.size() * 2) * 4), NP_ARCHIVE_FILE_ALIGNMENT);

      for (auto const &Child : m_Parent->m_Children) {
        Accumulator += NP_ARCHIVE_FILE_HEADER_SIZE;

        Processor.Write<uint32_t>((uint32_t)Accumulator);
        Processor.SeekRel(4);

        Accumulator += Child->m_Size;
      }
    }
  }
}
void NpArchive::DeserializeRecursive() {
  NpBinaryProcessor Processor(m_Bytes, m_Size);

  m_IsDirectory = CheckIfDirectory(&Processor);

  if (m_IsDirectory) {
    CreateDirectory(&Processor);

    for (auto const &Child : m_Children) {
      Child->DeserializeRecursive();
    }
  }
}

void NpArchive::ExtractToDiskRecursive(std::filesystem::path File) {
  if (m_IsDirectory) {
    if (m_Parent) {
      File /= CreateUniqueFileName();

      NpPathUtils::CreateDir(File, true);
    }

    for (auto const &Child : m_Children) {
      Child->ExtractToDiskRecursive(File);
    }
  } else {
    if (m_Size) {
      File /= CreateUniqueFileName();

      NpFileUtils::WriteBinary(File, m_Bytes, m_Size);
    }
  }
}
void NpArchive::UnfoldToDiskRecursive(std::filesystem::path File) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->UnfoldToDiskRecursive(File);
    }

    if (m_Parent) {
      if (m_Size) {
        File /= CreateUniqueFileName();

        NpFileUtils::WriteBinary(File, m_Bytes, m_Size);
      }
    }
  }
}
void NpArchive::PrintTableOfContentRecursive(std::function<void(char const *)> Callback, uint32_t Offset, uint32_t Indent, uint32_t Increment) {
  static char Buffer[1024] = {};

  if (m_Parent) {
    for (uint32_t I = 0; I < Offset; I++) {
      NP_ARCHIVE_PRINT(" ");
    }

    for (uint32_t I = 0; I < Indent; I++) {
      NP_ARCHIVE_PRINT("%c", ((I % Increment == 0) && (I >= Increment)) ? '|' : ' ');
    }

    uint64_t KiloByteInteger = m_Size / 1000;
    uint64_t KiloByteFraction = m_Size % 1000;

    NP_ARCHIVE_PRINT("0x%08X # %05u # %-20s # %-4s # %llu.%llu KB\n", m_ParentOffset, m_ParentIndex, m_Name, m_Type, KiloByteInteger, KiloByteFraction);
  }

  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->PrintTableOfContentRecursive(Callback, Offset, Indent + Increment, Increment);
    }
  }
}
void NpArchive::PrintOfTypeRecursive(std::function<void(char const *)> Callback, std::string const &Type) {
  static char Buffer[1024] = {};

  if (m_Parent) {
    if (std::strcmp(m_Type, Type.data()) == 0) {
      uint64_t KiloByteInteger = m_Size / 1000;
      uint64_t KiloByteFraction = m_Size % 1000;

      NP_ARCHIVE_PRINT("0x%08X # %05u # %-20s # %-4s # %llu.%llu KB\n", m_ParentOffset, m_ParentIndex, m_Name, m_Type, KiloByteInteger, KiloByteFraction);
    }
  }

  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->PrintOfTypeRecursive(Callback, Type);
    }
  }
}
void NpArchive::FindArchiveByTypeRecursive(std::string const &Type, NpArchive **Archive) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->FindArchiveByTypeRecursive(Type, Archive);
    }
  } else {
    if (std::strcmp(m_Type, Type.data()) == 0) {
      (*Archive) = this;

      return;
    }
  }
}
void NpArchive::FindArchiveByNameRecursive(std::string const &Name, NpArchive **Archive) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->FindArchiveByNameRecursive(Name, Archive);
    }
  } else {
    if (std::strcmp(m_Name, Name.data()) == 0) {
      (*Archive) = this;

      return;
    }
  }
}
void NpArchive::FindArchiveByTypeAndNameRecursive(std::string const &Type, std::string const &Name, NpArchive **Archive) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->FindArchiveByTypeAndNameRecursive(Type, Name, Archive);
    }
  } else {
    if (std::strcmp(m_Type, Type.data()) == 0 && std::strcmp(m_Name, Name.data()) == 0) {
      (*Archive) = this;

      return;
    }
  }
}
void NpArchive::FindArchivesByTypeRecursive(std::string const &Type, std::vector<NpArchive *> &Archives) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->FindArchivesByTypeRecursive(Type, Archives);
    }
  } else {
    if (std::strcmp(m_Type, Type.data()) == 0) {
      Archives.emplace_back(this);
    }
  }
}
void NpArchive::FindArchivesByNameRecursive(std::string const &Name, std::vector<NpArchive *> &Archives) {
  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->FindArchivesByNameRecursive(Name, Archives);
    }
  } else {
    if (std::strcmp(m_Name, Name.data()) == 0) {
      Archives.emplace_back(this);
    }
  }
}

bool NpArchive::CheckIfDirectory(NpBinaryProcessor *Processor) {
  if (m_KnownDirectoryTypes.contains(m_Type)) {
    Processor->SeekAbs(0);

    uint16_t EntryCount = (uint16_t)Processor->Read<uint32_t>();

    if (EntryCount > 0 && EntryCount < 0xFFFF) {
      for (uint32_t I = 0; I < EntryCount; I++) {
        if ((I * 4) >= (m_Size - 4)) {
          return false;
        }

        uint32_t Offset = Processor->Read<uint32_t>();

        if (Offset >= m_Size) {
          return false;
        }
      }

      for (uint32_t I = 0; I < EntryCount; I++) {
        if ((I * 4) >= (m_Size - 4)) {
          return false;
        }

        std::string type = NpStringUtils::RemoveChars(Processor->ReadStringRange(4), '\0');

        if (!m_KnownDirectoryTypes.contains(type) && !m_KnownFileTypes.contains(type)) {
          return false;
        }
      }

      return true;
    }
  }

  return false;
}
void NpArchive::CreateDirectory(NpBinaryProcessor *Processor) {
  Processor->SeekAbs(0);

  uint16_t EntryCount = (uint16_t)Processor->Read<uint32_t>();

  m_Children.resize(EntryCount);

  for (uint16_t I = 0; I < EntryCount; I++) {
    m_Children[I] = new NpArchive(this);
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    Child->m_ParentIndex = I;
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    Child->m_ParentOffset = Processor->Read<uint32_t>();
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    std::string Type = Processor->ReadStringRange(4);

    std::memcpy(Child->m_Type, Type.data(), 4);
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    if (Child->m_ParentOffset) {
      Processor->SeekAbs(Child->m_ParentOffset - NP_ARCHIVE_FILE_HEADER_SIZE);
      Processor->SeekRel(NP_ARCHIVE_FILE_HEADER_SIZE - 20);

      std::string Name = Processor->ReadStringRange(20);

      std::memcpy(Child->m_Name, Name.data(), 20);
    }
  }

  uint16_t PrevSequenceIndex = 0;

  for (uint16_t I = 1; I < EntryCount; I++) {
    NpArchive *CurrChild = m_Children[I];
    NpArchive *PrevChild = m_Children[PrevSequenceIndex];

    if (CurrChild->m_ParentOffset) {
      PrevChild->m_Size = CurrChild->m_ParentOffset - PrevChild->m_ParentOffset;
      PrevChild->m_SizePrev = PrevChild->m_Size;

      PrevSequenceIndex = I;
    }
  }

  if (EntryCount) {
    NpArchive *LastChild = m_Children[EntryCount - 1];

    LastChild->m_Size = m_Size - LastChild->m_ParentOffset;
    LastChild->m_SizePrev = LastChild->m_Size;
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    if (Child->m_Size >= NP_ARCHIVE_FILE_HEADER_SIZE) {
      Child->m_Size -= NP_ARCHIVE_FILE_HEADER_SIZE;
    }
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    Child->m_Bytes = new uint8_t[](ALIGN_UP_BY(Child->m_Size, NP_ARCHIVE_FILE_ALIGNMENT));

    std::memset(Child->m_Bytes, 0, ALIGN_UP_BY(Child->m_Size, NP_ARCHIVE_FILE_ALIGNMENT));
    std::memcpy(Child->m_Bytes, m_Bytes + Child->m_ParentOffset, Child->m_Size);

    Child->m_Size = ALIGN_UP_BY(Child->m_Size, NP_ARCHIVE_FILE_ALIGNMENT);
  }
}
void NpArchive::WriteDirectoryHeader(NpBinaryProcessor *Processor) {
  uint64_t Accumulator = 0;

  Accumulator += ALIGN_UP_BY(4 + ((m_Children.size() * 2) * 4), NP_ARCHIVE_FILE_ALIGNMENT);

  uint16_t EntryCount = (uint16_t)m_Children.size();

  Processor->Write<uint32_t>((uint32_t)EntryCount);

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    Accumulator += NP_ARCHIVE_FILE_HEADER_SIZE;

    Processor->Write<uint32_t>((uint32_t)Accumulator);

    Accumulator += Child->m_Size;
  }

  for (uint16_t I = 0; I < EntryCount; I++) {
    NpArchive *Child = m_Children[I];

    Processor->WriteStringRange(Child->m_Type, 4);
  }

  Processor->AlignUp(NP_ARCHIVE_FILE_ALIGNMENT);
}
void NpArchive::WriteFileContent(NpBinaryProcessor *Processor, NpArchive *Archive) {
  Processor->Write<uint64_t>(0x0);
  Processor->WriteStringRange(Archive->m_Type, 4);
  Processor->WriteStringRange(Archive->m_Name, 20);
  Processor->WriteByteRange(Archive->m_Bytes, Archive->m_Size);
}

uint64_t NpArchive::UpdateSizesRecursive() {
  uint64_t Accumulator = 0;

  if (m_IsDirectory) {
    Accumulator += ALIGN_UP_BY(4 + ((m_Children.size() * 2) * 4), NP_ARCHIVE_FILE_ALIGNMENT);

    for (auto const &Child : m_Children) {
      Accumulator += NP_ARCHIVE_FILE_HEADER_SIZE;
      Accumulator += Child->UpdateSizesRecursive();
    }
  } else {
    if (std::strcmp(m_Type, "ROF") == 0) {
      Accumulator = 8 + (m_Parent->m_Children.size() * 8);
    } else {
      Accumulator = m_Size;
    }
  }

  m_SizePrev = m_Size;
  m_Size = Accumulator;

  return m_Size;
}
void NpArchive::UpdateByteArraysRecursive() {
  if (m_Bytes) {
    if (m_Size != m_SizePrev) {
      uint8_t *Bytes = new uint8_t(m_Size);

      std::memset(Bytes, 0, m_Size);
      std::memcpy(Bytes, m_Bytes, std::min(m_Size, m_SizePrev));

      delete[] m_Bytes;

      m_Bytes = Bytes;
    }
  } else {
    m_Bytes = new uint8_t(m_Size);

    std::memset(m_Bytes, 0, m_Size);
  }

  if (m_IsDirectory) {
    for (auto const &Child : m_Children) {
      Child->UpdateByteArraysRecursive();
    }
  }
}

std::string NpArchive::CreateUniqueFileName() {
  std::string Name = m_Name;
  std::string Type = m_Type;

  if (Name != "" && Type != "") {
    return Name + "." + Type;
  } else if (Name != "" && Type == "") {
    return Name;
  } else if (Name == "" && Type != "") {
    return std::to_string(m_ParentIndex) + "." + Type;
  } else if (Name == "" && Type == "") {
    return std::to_string(m_ParentIndex);
  }

  return "";
}