# Zephyr LVGL Development Environment Setup
Development environment for STM32F746G-DISCO board using Zephyr RTOS and LVGL.

## Prerequisites

### 1. Windows System Requirements
- Windows 10 or newer
- PowerShell (comes with Windows)
- Administrator rights

### 2. Development Tools
1. Install Git:
   - Download from: https://git-scm.com/download/win
   - Use default installation options

2. Install Visual Studio Code:
   - Download from: https://code.visualstudio.com/
   - Install recommended extensions:
     * C/C++
     * CMake Tools
     * Cortex-Debug

3. Install STM32 Tools:
   - STM32CubeProgrammer: https://www.st.com/en/development-tools/stm32cubeprog.html
   - ST-LINK drivers: https://www.st.com/en/development-tools/stsw-link009.html

### 3. Python Environment Setup
1. Install Miniconda:
   - Download from: https://docs.conda.io/en/latest/miniconda.html
   - Choose Python 3.12 version
   - Run installer and add to PATH when prompted

2. Create and activate Conda environment:
```bash
# Create environment from yml file
conda env create -f zephyr_env_312.yml

# Activate environment
conda activate zephyr-env-312