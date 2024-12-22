#!/usr/bin/env sh

# Check for the OS type and platform
if [ "$1" = "web" ]; then
    echo "Building for Web (Emscripten)..."
    emcmake cmake . -B build-web -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
    emmake make -C build-web
    echo "Starting a local HTTP server for the Web build..."
    python3 -m http.server --directory build-web
elif [ "$(uname)" = "Darwin" ]; then
    # macOS
    echo "Building for macOS..."
    cmake . -B build -G "Unix Makefiles" -DPLATFORM=Desktop
    cmake --build build

    # Define app bundle structure
    APP_NAME="fluid-game"
    APP_DIR="${APP_NAME}.app"
    EXECUTABLE_NAME="fluid-game"

    echo "Creating macOS .app bundle..."
    mkdir -p "${APP_DIR}/Contents/MacOS"
    mkdir -p "${APP_DIR}/Contents/Resources"

    # Move the executable to the app bundle
    cp "build/${EXECUTABLE_NAME}" "${APP_DIR}/Contents/MacOS/"

    # Create Info.plist
    cat <<EOF > "${APP_DIR}/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>${EXECUTABLE_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>com.example.${APP_NAME}</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.9</string>
</dict>
</plist>
EOF

    echo "Refreshing app bundle..."
    /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -f "${APP_DIR}"

    echo "App bundle ${APP_DIR} is ready!"
    ./build/fluid-game
elif [ "$(uname)" = "Linux" ]; then
    # Linux
    echo "Building for Linux..."
    cmake . -B build -G "Unix Makefiles" -DPLATFORM=Desktop
    cmake --build build
    echo "Executable built at ./build/fluid-game"
elif echo "$OSTYPE" | grep -q "msys"; then
    # Windows with MinGW
    echo "Building for Windows..."
    cmake . -B build -G "MinGW Makefiles" -DPLATFORM=Desktop
    cmake --build build
    echo "Executable built at ./build/fluid-game.exe"
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi
