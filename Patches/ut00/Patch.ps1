Copy-Item -Path "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00_orig.dat" -Destination ".\ut00.dat"

.\ArchiveUtility.exe GetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\ut00.MD"
.\ModelUtility.exe GenerateConversionRules ".\ut00.MD" ".\Rules.json"
.\ModelUtility.exe ValidateFileAgainstRules ".\Tree.fbx" ".\Rules.json"
.\ModelUtility.exe ConvertFileIntoProprietaryFormat ".\Tree.fbx" ".\Rules.json" "Tree.MD"
.\ArchiveUtility.exe SetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\Tree.MD"

Copy-Item -Path ".\ut00.dat" -Destination "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00.dat"