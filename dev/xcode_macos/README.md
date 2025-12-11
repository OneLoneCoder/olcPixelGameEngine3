# olcPGE3 - OneLoneCoder Pixel Game Engine 3

Macos support

## Building the Project

### Requirements
- macOS 10.15 or later
- Xcode command line tools (`xcode-select --install`)

### Quick Start

1. **Open terminal in this directory**
2. **Build and run:**
   ```bash
   make debug
   ./build/debug/test_mh
   ```

### Build Options

- `make debug` - Build debug version
- `make all` - Build release version
- `make universal` - Build universal binary (ARM64 + Intel)
- `make clean` - Clean build files

### VS Code

Open this folder in VS Code for full IDE support with IntelliSense, debugging, and build tasks.

The project includes a complete VS Code workspace configuration with build tasks and debug support.

**Using the workspace file:**
```bash
code olcPGE3.code-workspace
```

### Xcode

You can also open the project in Xcode using the included project file:

```bash
open olcPGE3.xcodeproj
```

This will open the full Xcode project with all source files, build settings, and debugging capabilities.