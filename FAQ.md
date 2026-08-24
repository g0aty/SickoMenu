# Frequently Asked Questions

## List of Questions

1. [My antivirus shows some detections, is this malware???](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q1-my-antivirus-shows-some-detections-is-this-malware)
2. [What's the difference between release and debug?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q2-whats-the-difference-between-release-and-debug)
3. [How do I install the menu?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q3-how-do-i-install-the-menu)
4. [How do I open the menu?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q4-how-do-i-open-the-menu)
5. [How do I get rid of this error "SteamworksAuthFail"?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q5-how-do-i-get-rid-of-this-error-steamworksauthfail)
6. [Where do I find the Among Us folder?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q6-where-do-i-find-the-among-us-folder)
7. [Why is my menu not showing after I press delete?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q7-why-is-my-menu-not-showing-after-i-press-delete)
8. [Is there an "always impostor" feature in the menu?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q8-is-there-an-always-impostor-feature-in-the-menu)
9. [How do I run multiple instances of Among Us?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q9-how-do-i-run-multiple-instances-of-among-us)
10. [Does SickoMenu support other mods and is it planned?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q10-does-sickomenu-support-other-mods-and-is-it-planned)
11. [How do I update the menu myself by editing the source code?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q11-how-do-i-update-the-menu-myself-by-editing-the-source-code)
12. [Among Us updated, did SickoMenu update yet?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q12-among-us-updated-did-sickomenu-update-yet)
13. [What does "Reduce Host Anticheat (+25 Mode)" do? / Why am I not seeing any lobbies? / Why is no one joining my lobby?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q13-what-does-reduce-host-anticheat-25-mode-do--why-am-i-not-seeing-any-lobbies--why-is-no-one-joining-my-lobby)
14. [Can I Use SickoMenu on Among Us 3D?](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q14-can-i-use-sickomenu-on-among-us-3d)
15. [My antivirus keeps automatically deleting the file, how do I make it stop???](https://github.com/g0aty/SickoMenu/blob/main/FAQ.md#q15-my-antivirus-keeps-automatically-deleting-the-file-how-do-i-make-it-stop)

## Q1: My antivirus shows some detections, is this malware???
A: No and your life must be difficult, huh buddy? Some of those false-positives have "gen" in the detection label. Gen is short for generic and this means it's detected a pattern that's used commonly in all sorts of software. The pattern it's detecting is likely related to memory modification which is necessary for modifying the game like we do. Any detection label ending in "ml" is part of Microsoft Defender’s machine learning-based detection system, which uses patterns and behaviors to flag potential threats. If a file resembles known malware based on these patterns, it could be flagged even if it’s harmless. The compiled DLLs are built **by GitHub itself** directly from the code in the repository. Review the code. There's nothing hidden, and nothing **could** be hidden, it's fully open source. The code for the build automation is there too. In short, the detections are due to the fact that we modify the game's memory.

## Q2: What's the difference between release and debug?
A: Debug provides more detailed logs and a console window that spits out some helpful information. You should only need this if you're a developer or someone experiencing a specific, hard-to-diagnose bug. Most of the time you'll want the release version.

## Q3: How do I install the menu?
A: First of all, ensure that you have the latest version of the game, and that your version of the game is supported!. **We will not help you with an unsupported version of the game.**

## Q4: How do I open the menu?
A: Press the delete key on your keyboard. Your keyboard doesn't have a delete key? Google "how to press delete on <your keyboard's name>". Need a bit more help? Use SickoMenu once and then open your Among Us game directory and look for the sicko-config\default.json (or whatever config name you used) file. Open it and there's a section for keybinds. The values are [virtual key codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).
Note:
- Backspace and Delete are not the same thing, learn the difference before spamming issues. Still having trouble? Check Q7.
- Ensure that you are on the latest supported version of Among Us and are using the [latest version of SickoMenu](https://github.com/g0aty/SickoMenu/releases/latest). Look up the latest Among Us version if you don't know what it is. Check out the list of supported platforms on the main README!

## Q5: How do I get rid of this error "SteamworksAuthFail"?
A: Launch the game from Steam (not from executable); or make a text file next to Among Us.exe named "steam_appid.txt", open it, and save it with the content "943560" (NO QUOTES!).

## Q6: Where do I find the Among Us folder?
A: The location of your Among Us folder varies based on your platform, but here are some guides for Steam and Epic Games:

**Steam**: Right-click Among Us in your Library → Click `Manage` → Click `Browse local files`.
**Epic Games**: Right-click Among Us in your Library → Click `Manage` → Click the folder icon in the `Installation` box.
**Microsoft Store**: Go to `C:\Program Files\WindowsApps\` and search for Among Us.exe, the location of it is the required folder.
**XBOX App**: Right-click Among Us in your Library → Click Manage → Open the FILES tab → Click BROWSE... → Open the Among Us folder → Open the Content folder.

## Q7: Why is my menu not showing after I press delete?
A: Make sure you have downloaded the correct version of the menu depending on your platform (Steam, Itch, Epic, MS Store/XBOX App). If you already have, and
### If a watermark doesn't show on the main menu:
Make sure that **Disable fullscreen optimizations** is not checked in your **Among Us.exe** properties (Explorer).
### If a watermark shows on the main menu:
Ensure that you do not have any overlays other than your platform overlay enabled. Using DirectX overlays can cause the menu to not display. Overlays like MSI afterburner overlay can prevent the menu from showing. Steam overlay does not.

## Q8: Is there an "always impostor" feature in the menu?
A: Short answer: Kind of. Long answer: You can assign yourself the role you want before the match starts with the **host tab (only as host, but visible to everyone)**, it is **no longer possible without hosting**.

## Q9: How do I run multiple instances of Among Us?
A: Remove the `single_instance=` line from the `boot.config` file in the `Among Us_Data` folder in your Among Us directory. In case you want to join your own lobbies on official servers, use guest accounts with Quick Chat.

## Q10: Does SickoMenu support other mods and is it planned?
A: Mod support is **NOT** intended and will never be part of the menu. Other mods *can* detect SickoMenu usage and/or suspicious RPCs and prevent you from playing. Compatibility with other mods on your client is also **NOT** guaranteed.

## Q11: How do I update the menu myself by editing the source code?
A: So you recognize it's a technically complex task but still demand us to do it for you huh...
- Use [Il2CppInspectorRedux](https://github.com/LukeFZ/Il2CppInspectorRedux) for your platform
- Drag and drop GameAssembly.dll and global-metadata.dat
- Generate cpp scaffolding
- The appdata folder is what contains the changes we need
- Fixup the files to match the defines used by SickoMenu
	
## Q12: Among Us updated, did SickoMenu update yet?
A: First of all, we know. Secondly: Maybe, maybe not. You can always update it yourself. It's open source. Contributions are welcome. We are all doing this voluntarily. Do not beg for free labor. We'll update when we feel like updating. One day we'll stop feeling like it. Asking repeatedly pushes that day closer for many of us...

## Q13: What does "Reduce Host Anticheat (+25 Mode)" do? / Why am I not seeing any lobbies? / Why is no one joining my lobby?
A: It turns on modded protocol (host authority mode), which effectively reduces anticheat in your lobby (except for rate limits), however it prevents your lobby from being discovered in the lobby search menu by unmodded users.

## Q14: Can I Use SickoMenu on Among Us 3D?
A: Grow a brain. SickoMenu doesn't support Among Us 3D since it's a completely different game from Among Us. It is not planned as of now.

## Q15: My antivirus keeps automatically deleting the file, how do I make it stop???
A: Software is made to be used. It very often comes with some form of user interface. You, as a user, are meant to explore this user interface to familiarize yourself with the software you've installed on your computer... hint: it will likely be a menu named "exclusions" or "whitelist"... For Windows Security, use [this guide](https://www.elevenforum.com/t/add-or-remove-exclusions-for-microsoft-defender-antivirus-in-windows-11.8797/).