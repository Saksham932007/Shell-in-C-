#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <deque>
#include <signal.h>

extern char **environ;

// Global signal handler for Ctrl+C
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n[shell: Use 'exit' to quit]$ ";
        std::cout.flush();
    }
}

class Shell {
private:
    std::string currentDirectory;
    bool running;
    std::deque<std::string> commandHistory;
    static const size_t MAX_HISTORY = 100;

public:
    Shell() : running(true) {
        // Get initial working directory
        char* cwd = getcwd(nullptr, 0);
        if (cwd) {
            currentDirectory = std::string(cwd);
            free(cwd);
        } else {
            currentDirectory = "/";
        }
    }

    void run() {
        // Set up signal handler for Ctrl+C
        signal(SIGINT, signalHandler);
        
        std::cout << "Simple C++ Shell - Type 'help' for commands, 'exit' to quit\n";
        
        while (running) {
            printPrompt();
            
            std::string input;
            if (!std::getline(std::cin, input)) {
                // Handle EOF (Ctrl+D)
                std::cout << "\n";
                break;
            }
            
            // Skip empty input
            if (input.empty()) {
                continue;
            }
            
            executeCommand(input);
        }
    }

private:
    void printPrompt() {
        std::cout << "[shell:" << getShortPath() << "]$ ";
    }
    
    std::string getShortPath() {
        // Show only the last directory name for brevity
        size_t lastSlash = currentDirectory.find_last_of('/');
        if (lastSlash == std::string::npos || lastSlash == 0) {
            return currentDirectory;
        }
        return currentDirectory.substr(lastSlash + 1);
    }
    
    std::vector<std::string> parseCommand(const std::string& input) {
        std::vector<std::string> tokens;
        std::string current_token;
        bool in_quotes = false;
        bool escape_next = false;
        
        for (size_t i = 0; i < input.length(); ++i) {
            char c = input[i];
            
            if (escape_next) {
                current_token += c;
                escape_next = false;
            } else if (c == '\\') {
                escape_next = true;
            } else if (c == '"' || c == '\'') {
                if (in_quotes) {
                    in_quotes = false;
                } else {
                    in_quotes = true;
                }
            } else if (std::isspace(c) && !in_quotes) {
                if (!current_token.empty()) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
            } else {
                current_token += c;
            }
        }
        
        // Add the last token if it exists
        if (!current_token.empty()) {
            tokens.push_back(current_token);
        }
        
        return tokens;
    }
    
    void executeCommand(const std::string& input) {
        std::vector<std::string> args = parseCommand(input);
        
        if (args.empty()) {
            return;
        }
        
        // Add to history
        addToHistory(input);
        
        std::string command = args[0];
        
        // Handle built-in commands
        if (isBuiltinCommand(command)) {
            executeBuiltin(command, args);
        } else {
            executeExternalCommand(args);
        }
    }
    
    bool isBuiltinCommand(const std::string& command) {
        return command == "exit" || command == "cd" || command == "pwd" || 
               command == "help" || command == "echo" || command == "env" || 
               command == "export" || command == "unset" || command == "history";
    }
    
    void executeBuiltin(const std::string& command, const std::vector<std::string>& args) {
        if (command == "exit") {
            if (args.size() > 1) {
                int exit_code = std::atoi(args[1].c_str());
                std::cout << "Goodbye!\n";
                exit(exit_code);
            } else {
                std::cout << "Goodbye!\n";
                running = false;
            }
        } else if (command == "cd") {
            changeDirectory(args);
        } else if (command == "pwd") {
            std::cout << currentDirectory << std::endl;
        } else if (command == "help") {
            showHelp();
        } else if (command == "echo") {
            executeEcho(args);
        } else if (command == "env") {
            executeEnv(args);
        } else if (command == "export") {
            executeExport(args);
        } else if (command == "unset") {
            executeUnset(args);
        } else if (command == "history") {
            executeHistory(args);
        }
    }
    
    void changeDirectory(const std::vector<std::string>& args) {
        std::string targetDir;
        
        if (args.size() == 1) {
            // No argument - go to home directory
            const char* home = getenv("HOME");
            targetDir = home ? home : "/";
        } else {
            targetDir = args[1];
        }
        
        if (chdir(targetDir.c_str()) == 0) {
            // Successfully changed directory, update currentDirectory
            char* cwd = getcwd(nullptr, 0);
            if (cwd) {
                currentDirectory = std::string(cwd);
                free(cwd);
            }
        } else {
            perror("cd");
        }
    }
    
    void showHelp() {
        std::cout << "Built-in commands:\n";
        std::cout << "  help          - Show this help message\n";
        std::cout << "  exit [code]   - Exit the shell with optional exit code\n";
        std::cout << "  cd [dir]      - Change directory (no argument goes to home)\n";
        std::cout << "  pwd           - Print working directory\n";
        std::cout << "  echo [args]   - Display arguments\n";
        std::cout << "  env           - Display environment variables\n";
        std::cout << "  export VAR=val - Set environment variable\n";
        std::cout << "  unset VAR     - Remove environment variable\n";
        std::cout << "  history       - Show command history\n";
        std::cout << "\nAll other commands are executed as external programs.\n";
    }
    
    void executeEcho(const std::vector<std::string>& args) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) std::cout << " ";
            std::cout << args[i];
        }
        std::cout << std::endl;
    }
    
    void executeEnv(const std::vector<std::string>& /* args */) {
        char **env = environ;
        while (*env) {
            std::cout << *env << std::endl;
            env++;
        }
    }
    
    void executeExport(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cerr << "export: usage: export VAR=value\n";
            return;
        }
        
        std::string assignment = args[1];
        size_t equals_pos = assignment.find('=');
        
        if (equals_pos == std::string::npos) {
            std::cerr << "export: usage: export VAR=value\n";
            return;
        }
        
        std::string var_name = assignment.substr(0, equals_pos);
        std::string var_value = assignment.substr(equals_pos + 1);
        
        if (setenv(var_name.c_str(), var_value.c_str(), 1) != 0) {
            perror("export");
        }
    }
    
    void executeUnset(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cerr << "unset: usage: unset VAR\n";
            return;
        }
        
        if (unsetenv(args[1].c_str()) != 0) {
            perror("unset");
        }
    }
    
    void executeHistory(const std::vector<std::string>& args) {
        size_t start = 0;
        
        if (args.size() > 1) {
            int n = std::atoi(args[1].c_str());
            if (n > 0 && n < static_cast<int>(commandHistory.size())) {
                start = commandHistory.size() - n;
            }
        }
        
        for (size_t i = start; i < commandHistory.size(); ++i) {
            std::cout << (i + 1) << " " << commandHistory[i] << std::endl;
        }
    }
    
    void addToHistory(const std::string& command) {
        // Don't add empty commands or consecutive duplicates
        if (command.empty() || (!commandHistory.empty() && commandHistory.back() == command)) {
            return;
        }
        
        commandHistory.push_back(command);
        
        // Keep history size manageable
        if (commandHistory.size() > MAX_HISTORY) {
            commandHistory.pop_front();
        }
    }
    
    void executeExternalCommand(const std::vector<std::string>& args) {
        // Convert vector<string> to char* array for execvp
        std::vector<char*> cArgs;
        for (const auto& arg : args) {
            cArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        cArgs.push_back(nullptr);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            if (execvp(cArgs[0], cArgs.data()) == -1) {
                std::cerr << "shell: " << args[0] << ": command not found\n";
                exit(127); // Standard exit code for "command not found"
            }
        } else if (pid > 0) {
            // Parent process - wait for child to complete
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                return;
            }
            
            // Check if child process exited with an error
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0 && exit_status != 127) {
                    // Don't print for command not found (already handled in child)
                    // and don't print for successful commands
                }
            } else if (WIFSIGNALED(status)) {
                int signal = WTERMSIG(status);
                std::cerr << "Process terminated by signal " << signal << std::endl;
            }
        } else {
            // Fork failed
            perror("fork");
        }
    }
};

int main() {
    Shell shell;
    shell.run();
    return 0;
}