set PATH=c:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x86\
	cd 따라하기 08-1 쉐이더 파일
	fxc /Od /Zi /Ges /T vs_5_0 /E VS /Fo VS.fxo VS.fx
	fxc /Od /Zi /Ges /T ps_5_0 /E PS /Fo PS.fxo PS.fx
	cd ..