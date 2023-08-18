test on vs2022 in
•	GPU: NVIDIA GeForce RTX 3060 Laptop GPU, interfacing via PCIe and boasting support for SSE2 extensions.
•	Driver Version: 536.67
•	OpenGL Version: 4.6.0
•	GLSL (OpenGL Shading Language) Version: 4.60, as tailored to NVIDIA's proprietary implementation.
•	CPU: Intel(R) Core(TM) i7-12700H
•	RAM: 32GB running at 4800MHz
•	System：windows 11, version 22H2 

cmake -B build -G “Visual Studio 17 2022”
.\build\simpleopengl.sln

set 'test' for start.