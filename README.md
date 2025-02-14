# 👺 SickoMenu v4.1.2
A powerful utility for Among Us that aims to improve the game experience!

Join our very own Discord server for support, bug reports, and sneak peeks!
### Vanity Invite: https://discord.gg/sickos
### Permanent invite: https://dsc.gg/sickos

## ⚠️ Disclaimer
This project is intended for Educational Use only. I do not condone this software being used to gain an advantage against other people. This project is aimed to make Innersloth's anticheat better. Use at your own risk. If you get banned from playing entirely or from a lobby, I (g0aty) am not responsible.

This mod is not affiliated with Among Us or Innersloth LLC, and the content contained therein is not endorsed or otherwise sponsored by Innersloth LLC. Portions of the materials contained herein are property of Innersloth LLC. © Innersloth LLC.
## ⁉ Need Help?
Join Sicko Menu's Discord server and go to the Support channel to get help, [Sicko Menu Discord Invite Link](https://discord.gg/sickos)
## ⚙️ Features
A huge collection of various utilities and cheats such as
- Generate a brand new guest account every time the game is launched
- Fake Roles
- NoClip
- Move in Vents
- Zoom Out
- Confuser
- See Ghosts and Chat
- Better AUM Chat (Type in regular chat: "/aum [insert message here]")
- And much more! Check out the full list of features [here](https://github.com/g0aty/SickoMenu/blob/main/FEATURES.md)!

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
