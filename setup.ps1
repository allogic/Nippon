New-Item -Path ".\x64\Debug" -ItemType Directory -Force
New-Item -Path ".\x64\Release" -ItemType Directory -Force

Copy-Item -Path ".\Shaders\*" -Destination ".\x64\Debug" -Recurse
Copy-Item -Path ".\Shaders\*" -Destination ".\x64\Release" -Recurse

Copy-Item -Path ".\Config\*" -Destination ".\x64\Debug" -Recurse
Copy-Item -Path ".\Config\*" -Destination ".\x64\Release" -Recurse