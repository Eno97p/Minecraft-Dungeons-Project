rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"


rmdir /s /q .\Client\Default\ipch
rmdir /s /q .\Client\Default\Release
rmdir /s /q .\.vs\NewProject\v17\ipch

rmdir /s /q .\Client\Default\x64
rmdir /s /q .\Engine\Default\x64
rmdir /s /q .\IamGround\Default\x64


del /f /q .\Client\Bin\3DProject.exe
del /f /q .\Client\Bin\3DProject.ilk
del /f /q .\Client\Bin\3DProject.pdb

del /f /q .\IamGround\Bin\IamGround.pdb
del /f /q .\IamGround\Bin\IamGround.exe


del /f /q .\.vs\NewProject\v17\Browse.VC.db
del /f /q Framework.VC.db