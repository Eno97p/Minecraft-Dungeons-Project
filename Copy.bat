// 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치

xcopy			/y		.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy			/y		.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll				.\IamGround\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll				.\Effect_Tool\Bin\


xcopy			/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\

xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\IamGround\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\Effect_Tool\Bin\ShaderFiles\

xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\IamGround\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\Effect_Tool\Bin\ShaderFiles\