
#include <np_patch_pch.hpp>
#include <np_patch.hpp>

#include <np_core_file_util.hpp>

static void PatchSection(std::vector<uint8_t> &ImageBytes, std::string const &SectionName, std::vector<uint8_t> SectionBytes) {
#if defined(OS_WINDOWS)
  PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)ImageBytes.data();
  PIMAGE_NT_HEADERS64 NtHeaders = (PIMAGE_NT_HEADERS64)(ImageBytes.data() + DosHeader->e_lfanew);
  PIMAGE_FILE_HEADER FileHeader = &NtHeaders->FileHeader;
  PIMAGE_SECTION_HEADER SectionHeaders = (PIMAGE_SECTION_HEADER)((PBYTE)NtHeaders + sizeof(IMAGE_NT_HEADERS64));

  for (WORD SectionIndex = 0; SectionIndex < FileHeader->NumberOfSections; SectionIndex++) {
    PIMAGE_SECTION_HEADER SectionHeader = &SectionHeaders[SectionIndex];

    if (std::strcmp(SectionName.data(), (char *)SectionHeader->Name) == 0) {
      std::memcpy(ImageBytes.data() + SectionHeader->PointerToRawData, SectionBytes.data(), SectionBytes.size());
    }
  }
#elif defined(OS_LINUX)

#endif
}

int main(int Argc, char **Argv, char **Envp) {
  std::vector<uint8_t> ImageBytes = {};
  std::vector<uint8_t> SectionBytes = {};

  NpFileUtil::ReadBinary(Argv[1], ImageBytes);
  NpFileUtil::ReadBinary(Argv[3], SectionBytes);

  PatchSection(ImageBytes, Argv[2], SectionBytes);

  NpFileUtil::WriteBinary(Argv[4], ImageBytes);

  return 0;
}