# LAA-Patcher

A simple and lightweight Large Address Aware (LAA) patcher for Windows executables, built with Dear ImGui and DirectX 9.

LAA-Patcher allows you to check whether a 32-bit executable has the Large Address Aware flag enabled and apply the flag when needed through a clean and user-friendly interface.

## Features

* Check Large Address Aware (LAA) status
* Enable LAA on supported executables
* Simple and modern graphical interface
* Lightweight and portable
* No external dependencies required at runtime

## Usage

1. Close the target application.
2. Launch LAA-Patcher.
3. Select an executable file (`.exe`).
4. Review the current LAA status.
5. Enable LAA if it is not already enabled.

## What is Large Address Aware?

Large Address Aware is a flag in Windows executables that allows compatible 32-bit applications to access more memory on 64-bit operating systems. This can help reduce memory-related issues in some applications and games.

## Disclaimer

- Always create a backup of the original executable before modifying it, app itself will get a backup file with (`.laabackup`) extension, but its better for you to do it just in case.
- While the patching process is straightforward, modifying executable files is done at your own risk.

## Support

If you find this project useful, consider giving the repository a ⭐ on GitHub. It helps others discover the project and motivates future improvements.

Bug reports, feature suggestions, and contributions are always welcome.

## Credits

* Dear ImGui by ocornut
* UI design inspiration by [@lbyenjoyer](https://github.com/lbyenjoyer) (Design for Older Versions)