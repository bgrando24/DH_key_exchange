# C++ implementation of the Diffie-Hellman key exchange algorithm

## Quick overview

The Diffie-Hellman key exchange (`DHKE`) algorithm is used for establishing a secure method of communication over a public/unsecure network.

Let's examine a (very) simplified explanation of how it works, using "Alice" and "Bob" as our two communicating individuals:

1. It starts with a key exchange process, where Alice and Bob agree on a shared set of numbers (parameters). **At this point, all communication is unencrypted and can be viewed by anyone**.
2. Both Alice and Bob also have their own super secret keys that they haven't told anyone about, not even each other.
3. Alice and Bob both take the shared parameters and combine it with their respective secret keys, giving them a new key.
4. Then, Alice and Bob swap their newly generated keys with each other.
5. Finally, Alice takes the key Bob just gave her and combines it with her secret key again, giving her the final key. Bob does the exact same with the generated key Alice just gave him.
6. **Even though neither of them knew the other's secret key, both Alice and Bob now have the exact same final key**.
7. This "final key" can then be used in a standard encryption method (e.g. AES) to now send encrypted data securely.

### But what if someone was listening to their initial key exchanges?

The benefit of `DHKE` is that it doesn't matter if someone was listening to the initial key exchange. The 'eavesdropper' could even attempt to create their own "final key", but it won't match the final key that both Alice and Bob generated.

The key aspect is that **the eavesdropper doesn't know either of Alice's or Bob's secret keys**, because they were never shared. Yet without either of those secret keys, no one can generate the exact same "final key" that Alice and Bob are able to generate.

The security comes from the same idea behind most modern encryption algorithms: it involves a **"very hard problem"**. Even if you had the data from the initial key exchange, plus the encrypted communication data, trying to reverse the process used to generate the "final key" is so computationally expensive that it can't reasonably be done by any modern classical computer.

## Prerequisites (run once per machine)

Install the usual build tools, CMake ≥ 3.25, Git, and Ninja (preferred). Example commands:

**Ubuntu/Debian**

```sh
sudo apt update
sudo apt install -y build-essential cmake ninja-build git curl pkg-config
```

**macOS (Homebrew)**

```sh
xcode-select --install
brew install cmake ninja git
```

**Windows (MSVC)**

1. Install: Visual Studio Build Tools (C++), Git, CMake, (optional) Ninja.
2. Open an **x64 Native Tools Command Prompt for VS** (or run `vcvarsall.bat`) so `cl.exe` is on PATH.
    - If you prefer MinGW instead of MSVC, set `VCPKG_TARGET_TRIPLET=x64-mingw-dynamic` before configuring.

---

## Quick start (new project)

1. Clone the skeleton and rename the remote when ready:
    ```sh
    git clone https://github.com/bgrando24/cpp_skeleton.git my_new_project
    cd my_new_project
    ```
2. Pull submodules and bootstrap vcpkg:

    ```sh
    git submodule update --init --recursive
    ./vcpkg/bootstrap-vcpkg.sh
    # Windows PowerShell: .\vcpkg\bootstrap-vcpkg.bat
    ```

    > **Windows tip:** Run the commands from an **MSVC Developer Prompt** so CMake/Ninja use `cl.exe`
    > (matches vcpkg’s `x64-windows` triplet). If you use MinGW, set `VCPKG_TARGET_TRIPLET=x64-mingw-dynamic`.

3. Configure and build using the provided CMake presets:

    ```sh
    cmake --preset release                  # or --preset debug
    cmake --build --preset build-release    # or build-debug
    ```

    - If you get a “could not read presets” error on older CMake, either upgrade CMake (≥3.27 for Presets v8)
      or edit `CMakePresets.json` and set `"version": 6`.

4. Run the sample app:
    ```sh
    ./build/app                             # Windows: .\build\app.exe
    ```

During development you only need `cmake --build --preset build-<config>` to rebuild.

---
