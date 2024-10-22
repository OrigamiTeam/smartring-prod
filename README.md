# SmartRing Firmware 🟣
<img src="https://github.com/OrigamiTeam/SmartRing_FV_FW/assets/165775110/cd0ca7c8-dbfd-4871-b7d5-e363b4d39d7f.png" width="1000">

## Cloning the Repository

   > :warning: Since the **AWS Library components** are linked repository folders we need to clone the project recursively **(This is an important step)**.
```c
git clone git@github.com:OrigamiTeam/SmartRing_FV_FW.git --branch v3.0.5_pt --recursive
```

# Installation: Windows 💻

## 1. Install ESP-IDF v4.4.2
The version used on the project is **ESP-IDF v4.4.2**. This version is an old release so needs to be downloaded directly from the [Espressif ESP IDF Github Repository](https://github.com/espressif/esp-idf/releases/tag/v4.4.2).

## 1.1 Using Git
   * To get this release, use the following commands:
```c
mkdir ~/esp
cd ~/esp
git clone -b v4.4.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.4.2
cd esp-idf-v4.4.2/
```
This is the recommended way of obtaining v4.4.2 of ESP-IDF.

## 1.2 Using .rar download file
This `esp-idf-v4.4.2.zip` archive includes a `.git` directory and all the submodules, so can be used out of the box. This archive is provided for users who have connectivity issues preventing them from cloning from GitHub.

   * This archive can be downloaded from Espressif's download server:  [https://dl.espressif.com/github_assets/espressif/esp-idf/releases/download/v4.4.2/esp-idf-v4.4.2.zip](https://dl.espressif.com/github_assets/espressif/esp-idf/releases/download/v4.4.2/esp-idf-v4.4.2.zip)
   * After downloading the .zip, extract it to a new created directory `/esp`. The folder `/esp` should have the `esp-idf-v4.4.2` file inside.

More in-depth information [here](https://github.com/espressif/esp-idf/releases/tag/v4.4.2).

## 2. Install VSCode and ESP-IDF Extension

Follow the **Official Espressif Tutorial** for Installing and Setting the ESP-IDF Extension: [VSCode and ESP-IDF Tutorial](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md).

:warning: You should select **"Using existing setup"** and select the previously downloaded **ESP-IDF v4.4.2 Version**.

After selecting the previously downloaded version, the ESP-IDF extension will download the remaining files needed. After this, the installation is done.


# Installation: Linux 💻

## Install Cmake Build System

## Make ESP directory:
```c
  mkdir -p ~/esp
  cd ~/esp
  git clone -b v4.4.2 --recursive https://github.com/espressif/esp-idf.git
```

## Install Packages (for Ubuntu and Debian):
   * Install all packages
```c   
sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing cmake ninja-build ccache
```
   * If error is found try  `alias python=python3` or 
   * Change all `python` in the install packages to `python3`
   * You can install other useful packages listed below 
     1. `sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0`

## Toolchain Setup (This is not necessary)
   * Download Espressif toolchain (download latest version)
      1. for Linux(64-bit): https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
      2. for Linux(32_bit): https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-80-g6c4433a-5.2.0.tar.gz
   * Extract the file
     1. `cd ~/esp`
     2. `tar -xzf ~/Downloads/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz`
     3. Update `PATH` environment variable by adding the following line to your `~/.profile`
         1. `export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"`
         2. `alias get_esp32='export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"'`
     4. Log off and Log in back to make `.profile` changes effective. Run printenv `PATH` to verify if the `PATH` is correctly set

## Install and set env variables 
   * `cd ~/esp/esp-idf`
   * `./install.sh`
   * `./export.sh`

# Building and Compiling: Windows 🛠️
⚠️Make sure that your project has the sdkconfig file correctly and well set, this step is very important to assure the correct building and compiling of the project

## Using ESP-IDF to build and compile
   * Go to the VSCode side panel and open the **ESP-IDF Explorer Menu**.
   * Set the **Espressif Target (ESP32 Custom Target and press Enter)**.
   * Click the **Build** button.
   * At this point, your project should successfully compile.

**Any encountered erros/warnings will be shown on the terminal window.**

# Building and Compiling: Linux 🛠️

## Using CMAKE to build and compile
   * Go to the cloned SmartRing folder and add the following command
     1. `mkdir build`
     2. `cmake ..`
     3. `make -j4`
   * At this point, your project should successfully compile.
  
## Alternatively, just incase cmake fails to build directly 
   * In the terminal where you are going to use ESP-IDF, run `. $HOME/esp/esp-idf/export.sh` and then
     1. you can use `idf.py build` in your project directory to build
     2. and `idf.py -p PORT -b 115200 flash` to flash on the hardware
   * For more installation details, please go to [Espressif Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#step-4-set-up-the-environment-variables)

----------------------------------------------------------

📂[Documentation FTDI and ESP32 Connection](https://docs.google.com/document/d/1YYdHxt1JrCmewbVLLrnRIHE93f_jnYob0VQ80Za_zIs/edit)

📂[Documentation Flashing Firmware ESP32](https://docs.google.com/document/d/1rKvC46A_gp3_Xjb95rSW--5flFFYYkcRsMTqLp5N3Z0/edit)

📂[Documentation Functional Test Procedure](https://docs.google.com/document/d/1h_vyS2Fxy08jIg7UomDqoLVQfKDoY_yTb4FMmEFiZNI/edit#heading=h.fqme9tx3j7kf)

