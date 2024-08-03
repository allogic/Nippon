Copy-Item -Path "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\vt\vt17_orig.dat" -Destination ".\vt17.dat"

.\ArchiveUtility.exe GetByTypeAndName ".\vt17.dat" "MD" "vt17" ".\vt17.MD"
.\ModelUtility.exe GenerateConversionRules ".\vt17.MD" ".\Rules.json"
.\ModelUtility.exe ValidateFileAgainstRules ".\Tree.fbx" ".\Rules.json"
.\ModelUtility.exe ConvertFileIntoProprietaryFormat ".\Tree.fbx" ".\Rules.json" "Tree.MD"
.\ArchiveUtility.exe SetByTypeAndName ".\vt17.dat" "MD" "vt17" ".\Tree.MD"

Copy-Item -Path ".\vt17.dat" -Destination "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\vt\vt17.dat"