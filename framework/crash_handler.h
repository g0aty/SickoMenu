#pragma once

// Installs the SickoMenu unhandled-exception crash handler.
// Call once at startup (top of Run()).
// On crash, writes a detailed report to %TEMP%\SickoMenu_crash.log
// including exception type, registers, and a full stack trace with
// file names and line numbers (requires SickoMenu.pdb next to the DLL).
void InstallSickoCrashHandler();
