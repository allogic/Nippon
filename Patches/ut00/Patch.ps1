Copy-Item -Path "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00_orig.dat" -Destination ".\ut00.dat"

..\..\x64\Debug\ArchiveUtility.exe GetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\ut00.MD"
..\..\x64\Debug\ModelUtility.exe GenerateConversionRules ".\ut00.MD" ".\Rules.json"
..\..\x64\Debug\ModelUtility.exe ValidateFileAgainstRules ".\Tree.fbx" ".\Rules.json"
..\..\x64\Debug\ModelUtility.exe ConvertFileIntoProprietaryFormat ".\Tree.fbx" ".\Rules.json" "Tree.MD"
..\..\x64\Debug\ArchiveUtility.exe SetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\Tree.MD"

Copy-Item -Path ".\ut00.dat" -Destination "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00.dat"