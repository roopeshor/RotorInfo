# Rotor Speed

Reads serial data from arduino and displays it, does stuff with it.

# Build and run
## 1. Get source
```bash
git clone --depth 1 https://github.com/roopeshor/RotorInfo
cd RotorInfo
```
## 2. Build and run
### On windows
```bash
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\win\rotorinfo.exe
```

### On linux
```bash
cmake -S . -B build
make -C build -j12
./build/linux/rotorinfo
```

# Deploying
Add all required `dll` files to `build/win` directory with [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html):
```bash
cd build/win
windeployqt --no-translations rotorinfo.exe
```

the `build/win` directory can be compressed and shared to other systems.