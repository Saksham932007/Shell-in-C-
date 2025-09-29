# C++ Shell

A fully functional shell implementation in C++ that can execute any command, with support for built-in commands, command history, environment variable management, and proper error handling.

## Features

- **Command Execution**: Execute any external command available in PATH
- **Built-in Commands**: Essential shell built-ins like `cd`, `pwd`, `echo`, `env`, `export`, `unset`, `history`, `help`, and `exit`
- **Command Parsing**: Handles quoted strings, escaped characters, and complex arguments
- **Command History**: Track and display command history with the `history` command
- **Environment Variables**: Set, unset, and display environment variables
- **Error Handling**: Proper error messages and exit codes
- **Signal Handling**: Graceful handling of Ctrl+C (SIGINT)
- **Cross-Platform**: Works on Unix-like systems (Linux, macOS)

## Building

### Using Make (Recommended)
```bash
make
```

### Manual Compilation
```bash
g++ -std=c++17 -Wall -Wextra -g -o shell shell.cpp
```

### Other Make Targets
- `make debug` - Build with debugging symbols
- `make release` - Build optimized release version
- `make clean` - Remove build artifacts
- `make run` - Build and run the shell
- `make install` - Install to /usr/local/bin (requires sudo)

## Running

```bash
./shell
```

Or after installation:
```bash
shell
```

## Built-in Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `help` | Show help message | `help` |
| `exit` | Exit the shell | `exit [code]` |
| `cd` | Change directory | `cd [directory]` |
| `pwd` | Print working directory | `pwd` |
| `echo` | Display text | `echo [args...]` |
| `env` | Show environment variables | `env` |
| `export` | Set environment variable | `export VAR=value` |
| `unset` | Remove environment variable | `unset VAR` |
| `history` | Show command history | `history [n]` |

## Examples

```bash
# Navigate directories
cd /home/user
pwd
cd ..

# Environment variables
export MY_VAR=hello
echo $MY_VAR
env | grep MY_VAR
unset MY_VAR

# Command history
history
history 10

# External commands
ls -la
cat file.txt
gcc --version
python3 script.py

# Quoted arguments
echo "Hello World"
mkdir "My Folder"
```

## Technical Details

### Architecture
- **Parsing**: Robust command line parsing with support for quotes and escapes
- **Process Management**: Uses `fork()` and `execvp()` for external command execution
- **Signal Handling**: Catches SIGINT (Ctrl+C) to prevent accidental shell termination
- **Memory Management**: Proper cleanup and memory management throughout

### Error Handling
- Command not found errors
- Permission denied errors
- Invalid arguments for built-in commands
- System call failures

### Limitations
- No pipe (`|`) or redirection (`>`, `<`) support
- No job control (background processes with `&`)
- No command substitution or variable expansion
- No scripting capabilities

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+)
- Unix-like operating system (Linux, macOS, WSL)
- Standard POSIX libraries

## License

This project is provided as-is for educational purposes.
