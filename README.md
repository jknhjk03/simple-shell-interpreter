# Simple Shell Interpreter (SSI)

A custom Unix-like shell implemented in C.  
This shell supports running commands, managing foreground and background processes, handling I/O redirection, and saving command history between sessions.

---

## Features
- **Foreground & Background Process Execution**  
  - Run commands normally or use `bg` to launch jobs in the background.
  - Example:  
    ```bash
    bg sleep 10
    ```
- **Background Job Tracking**
  - `bglist` shows currently running background jobs.
- **I/O Redirection**
  - Output: `>` overwrite, `>>` append  
    Example:  
    ```bash
    ls -la > output.txt
    ```
  - Input: `<` read from a file  
    Example:  
    ```bash
    sort < data.txt
    ```
- **Command History**
  - Command history is saved between sessions using GNU Readline.
  - Navigate with **↑ / ↓** arrow keys.

---

## Installation & Build
### Prerequisites
- **Linux or macOS**  
- GCC Compiler  
- GNU Readline library

#### Install Readline (macOS):
```bash
brew install readline
```
## Install Readline (Ubuntu/Debian)

## Example Usage
james@MacBook: ~/ssi-project > ls
Makefile  src  README.md

james@MacBook: ~/ssi-project > bg sleep 5
[1] PID: 12345

james@MacBook: ~/ssi-project > bglist
[1] Running PID: 12345

james@MacBook: ~/ssi-project > exit
