# 👺 SickoMenu v4.1.2
A powerful utility for Among Us designed to enhance your game experince with custom features!

**⚠️ Intended for educational and experimental use only.**

Join our very own Discord server for support, bug reports, and sneak peeks!
### Vanity Invite: https://discord.gg/sickos
### Permanent invite: https://dsc.gg/sickos
---
## ⚠️ Disclaimer
This project is **strictly for educational purposes** to study game mechanics and anti-cheat systems. It is **not endorsed by Innersloth LLC**, and we strongly discourage use in public matches. By downloading, you accept full responsibility for any consequences.I (g0aty) am not responsible for any repercussions.

**Legal Notice:**  
Portions of materials used are property of Innersloth LLC. © Innersloth LLC. This mod complies with Innersloth's [Modding Policy](https://www.innersloth.com/policies/) for non-malicious private use.

## 🛑 Ethical Use Protocol
**By using SickoMenu, you agree to:**
1. **Use exclusively in private lobbies** with consenting players.
2. **Never exploit features in public matches** or to harass others.
3. **Respect Innersloth's Terms of Service** and the integrity of the game.
4. **Immediately disable the mod** (`PAUSE BREAK` hotkey) if joining public lobbies accidentally.

> 🚫 Misuse may result in account bans. **We do not condone cheating.**  
> Report unethical usage in our [Discord](https://discord.gg/sickos).

---

## ⚙️ Features
A huge amount of features!!
  - NoClip (`CTRL`)  
  - Ghost Visibility  
  - Confuser (May disrupt gameplay)  
- **Cosmetic/UI Enhancements**:  
  - Zoom Out  
  - Better AUM Chat (`/aum [message]`)  
- **Full Feature List**: [FEATURES.md](https://github.com/g0aty/SickoMenu/blob/main/FEATURES.md)  

> 🔸 Features marked with **!** may impact game balance. Always obtain lobby consent

## 📸 Screenshot
<p align="center">
   <img src="screenshot.png">
</p>

## 👌 Supported Versions
- ✅ Steam (Supported)
- ✅ Epic Games (Supported)
- ✅ itch.io (Supported)
- ❓ Cracked (works occasionally, I don't condone it)
- ❌ Microsoft Store (Not Supported)
- ❌ iOS/iPadOS/Android (Not Supported)
- ❌ Switch/Xbox/Playstation (Not Supported)

## ⬇️ Download & Install
### For Windows
You can find the latest release [here](https://github.com/g0aty/SickoMenu/releases/latest).
Either inject `SickoMenu.dll` with a reliable injector or put `version.dll` in your Among Us directory (the folder containing `Among Us.exe`).

### For Proton (Version Proxy Only)
First you will need [protontricks](https://github.com/Matoking/protontricks), you can install it with your packager of choice.

1. Make sure you are running Among Us under Proton. 
   On Steam you can check this by going to **Properties -> Compatibility**
2. Put version.dll into your Among Us directory (the folder containing `Among Us.exe`).
3. Run `protontricks --gui`
4. Choose **Among Us**
5. Click on **Select the default wineprefix** and then **OK**
6. Click on **Run winecfg** and then **OK**
7. In the configuration window, click on **Libraries**
8. Enter `version` into the **New override for library** input field
9. Click **Add** and then **Apply**
10. SickoMenu should now work properly in the game

### For macOS (Version Proxy Only)

> [!NOTE]
> This method uses CrossOver. If you have ANY other ways to use this with other software, refer to the contributing section!

1. Install a bottle of CrossOver, install Steam in it and install Among Us.
2. Open the C drive inside of CrossOver (there should be a button). Quit CrossOver. A Finder window will open.
3. Go to `Program Files (x86)/Steam/steamapps/common/Among Us/` and put the `version.dll` into it. Then reopen CrossOver.
5. Go to the **Configure Wine** panel, go to **Libraries**, open the menu below **New Replacement for:** and search for `version`.
6. Select it and click **Add**, then **Apply** and **OK**.
7. Start Among Us and press **Command+⌦** to show SickoMenu.

## ⌨️ Default Hotkeys
- Show Menu - DELETE
- Show Radar - INSERT
- Show Console - HOME
- Show Replay - END
- Repair Sabotage - PAGE DOWN (PgDn)
- NoClip - CTRL
- Panic / Disable SickoMenu - PAUSE BREAK (Break)

## ⚒️ Compile (Configurations)
You can compile two different versions of the menu. Normal or Version Proxy. Steps to compile can be found [here](https://docs.google.com/document/d/1bdXyasr7suassff_or3ywPyItGkjhlTfbBJtvaJ6udQ/edit?usp=sharing).

### Normal (SickoMenu.dll)
Inject it with any injector you have.

- Debug (With Debug Information and can be attached to process)
- Release (Optimized with all information stripped)

### Version Proxy (version.dll)
Will automatically be loaded by the Game itself if the dll is in the game directory.

- Debug_Version (With Debug Information and can be attached to process)
- Release_Version (Optimized with all information stripped)

## 🙏 Special Thanks
* The BitCrackers team for creating [AmongUsMenu](https://github.com/BitCrackers/AmongUsMenu)
* [KulaGGin](https://github.com/KulaGGin) (Helped with some ImGui code for replay system)
* [tomsa000](https://github.com/tomsa000) (Helped with fixing memory leaks and smart pointers)
* [cddjr](https://github.com/cddjr) (Helped in updating to the Fungle map, saved a lot of my time)
* Everyone else who contributed to the code and I couldn't list here. Thank you!

## 💁 Contributing
1. Fork it [here](<https://github.com/g0aty/SickoMenu/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

## ✨ Inspiration
* [DarkModeAU](https://github.com/the-real-techiee/DarkModeAU) by the-real-techiee
* [YuEzTools](https://github.com/Team-YuTeam/YuEzTools) by Team-YuTeam
* [BetterAmongUs-Public](https://github.com/EnhancedNetwork/BetterAmongUs-Public) by EnhancedNetwork (D1GQ)
* [MalumMenu](https://github.com/scp222thj/MalumMenu) by scp222thj
