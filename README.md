# 🖼️ Image Processing Tasks

This repository contains simple C++ image processing tasks using OpenCV.

---

## ✅ Task 1 – Převod barevných modelů a konvoluce

### 🔧 Compilation on Merlin
```bash
g++ -o mt01 mt01.cpp -Wall -I/usr/local/include/opencv4 -L/usr/local/lib64 \
    -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

./mt01 lena.png
```
🧾 Output
- Average error
- Počet rozdílných pixelů
- Maximální rozdíl



## ✅ Task 2 – Horní a dolní propusť pomocí DFT
```bash
g++ -o mt03 mt03.cpp -Wall -I/usr/local/include/opencv4 -L/usr/local/lib64 \
    -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

./mt03 garden.png 20
```
