Copy-Item -Path "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\vt\vt17_orig.dat" -Destination ".\vt17.dat"

..\..\x64\Debug\ArchiveUtility.exe GetByTypeAndName ".\vt17.dat" "MD" "vt17" ".\vt17.MD"
..\..\x64\Debug\ModelUtility.exe GenerateConversionRules ".\vt17.MD" ".\Rules.json"
..\..\x64\Debug\ModelUtility.exe ValidateFileAgainstRules ".\Tree.fbx" ".\Rules.json"
..\..\x64\Debug\ModelUtility.exe ConvertFileIntoProprietaryFormat ".\Tree.fbx" ".\Rules.json" "Tree.MD"
..\..\x64\Debug\ArchiveUtility.exe SetByTypeAndName ".\vt17.dat" "MD" "vt17" ".\Tree.MD"

Copy-Item -Path ".\vt17.dat" -Destination "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\vt\vt17.dat"