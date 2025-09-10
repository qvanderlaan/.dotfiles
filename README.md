# 🏠 My Dotfiles

Personal dotfiles configuration for a streamlined development environment.

## 🚀 Installation

### Prerequisites
- `zsh` shell
- `git`
- `xclip` (for clipboard operations)
- `curl` (for downloading)

### Setup Instructions

1. **Clone the repository:**
   ```bash
   git clone <your-repo-url> ~/.dotfiles
   ```

2. **⚠️ Important: Setup .zshrc**

   You need to either **link** or **source** the `.zshrc` file:

   **Option A - Create a symlink (recommended):**
   ```bash
   ln -sf ~/.dotfiles/.zshrc ~/.zshrc
   ```

   **Option B - Source in your existing .zshrc:**
   ```bash
   echo "source ~/.dotfiles/.zshrc" >> ~/.zshrc
   ```

3. **Reload your shell:**
   ```bash
   source ~/.zshrc
   ```

## 🛠️ Components

### Shell Configuration (`.zshrc`)
- **Custom prompt:** Simple `%~$ ` format showing current directory
- **Aliases:**
  - `norm`: Runs norminette with CheckForbiddenSourceHeader rule
  - `comp`: Compiles with strict flags (`-Wall -Werror -Wextra`)
- **Path additions:**
  - Adds `~/.dotfiles/Commands` to PATH for custom commands
  - Includes various development paths and libraries

### Commands Directory
Custom utilities available system-wide (added to PATH):

#### `copy_changes`
Copies the current git diff to clipboard using `xclip`.
```bash
copy_changes
```

#### `copy_context`
Copies all text files in the repository to clipboard with headers. Falls back gracefully if `tree` is not installed.
```bash
copy_context
```

#### `cursor`
Manages Cursor editor installation and updates:
```bash
cursor --install    # Install Cursor editor
cursor --update     # Update Cursor editor
cursor --help       # Show help
cursor [args...]    # Launch Cursor with arguments
```

#### `nah`
Safe git reset with confirmation prompt:
```bash
nah  # Prompts before running git reset --hard
```

### Helpers Directory

#### `colors.sh`
Provides colored output functions for scripts:
- `info()` - Cyan colored informational messages
- `success()` - Green colored success messages with ✅
- `warning()` - Yellow colored warnings with ⚠️
- `error()` - Red colored error messages with ❌

### AppImages Directory
Stores portable application binaries. Currently includes:
- **Cursor editor:** Downloaded and managed by the `cursor` command

## 🎯 Usage Examples

```bash
# Compile with strict flags
comp myfile.c -o myfile

# Check code style
norm myfile.c

# Copy current changes to clipboard
copy_changes

# Copy entire project context to clipboard
copy_context

# Install/update Cursor editor
cursor --install
cursor --update

# Reset git changes safely
nah

# Open Cursor editor
cursor
cursor myproject/
```
