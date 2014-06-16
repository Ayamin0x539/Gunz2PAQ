Gunz2PAQ
========

 PseudoAutoQuester: A dll that makes Gunz 2 memory edits.

VERSION 2.07
--------------

**[FEATURES]**
- Compatibility with Cyanide. You can inject this with Cyanide. Actually, it's meant to be used with Cyanide. I purposefully made it so the hotkeys wouldn't conflict.
- Infinite Ammo
- Infinite z-bar (hit something/get hit first and it'll jump up)
- No cooldown on special z-bar skills
- Monster vacuum in quests (and multiplayer if you're host... it's VERY noticeable though)
- Only supports the second quest map
- Teleportation to rally points
- Up/down teleportation (hilariously not working) - actually, this is fixed.
- Plays a C major progression upon activation
- Plays a C minor progression upon deactivation

**[HOTKEYS]**

*NUMPAD 7*
- Toggle infinite Z-Bar and no cooldown on Z-Bar skills

*NUMPAD 8*
- Toggle teleportation

*NUMPAD 9*
- Toggle infinite ammo

*--- TELEPORTATION ENABLED ---*

*Left Arrow Key*
- Teleports you to first rally point in Fallen Angel

*Right Arrow Key*
- Teleports you to last rally point in Fallen Angel

*Up and Down arrow keys*
- Will send you up or down (fixed!)


**[INSTRUCTIONS]**
- Inject the dll.
- Host a quest game (7th stage only).
- Run past the green truck until you trigger the dialogue.
- Teleport to the first rally point.
- Kill at least one monster so you get the gold at the end.
- Hide behind the helecopter and position the mobs in the fire.
- Afk.
- Teleport to the last rally point.
- Enjoy.

**[CHANGELOG]**

V2.07 (June 2, 2014)
- Vacuum doesn't work anymore
- Changed hotkeys to be compatible with Cyanide
- Updated addresses to work with most recent friendlist-fix update

V2.06 (May 27, 2014)
- Made an all-in-one hotkey for vacuum (NUMPAD 9)
- Updated addresses according to the update

V2.05 (May 19, 2014)
- Fixed up and down teleportation
- First teleport will bring you to the safespot immediately

V2.04 (May 19, 2014)
- Removed NUMPAD9 hotkey
- Changed hotkeys

V2.03 (May 19, 2014)
- Changed teleports to support Fallen Angel only

V2.02 (May 16, 2014)
- Teleports should register
- Attempted a fix for up/down teleportation
- Less CPU intensive

V2.01 (May 16, 2014)
- Fixed hotkeys; they should better now (thanks killer1478)
- Replaced message boxes with beeps
- Sorry if the beep sequences are a bit long, I was having fun D:

V2.00 (May 15, 2014)
- HOTKEY REVAMP! Woo!
- Fixed the logic a bit in the loop.
- Lowered the sleep delay in between each loop. Let me know if you're having lag issues.
- Added up/down teleport options that don't work. I'll try to figure this out.
- Fixed the second teleport in the second map. It was sending you below the elevator.
- The distance mobs get vacuumed to you is now greatly increased so you can stay alive. I'll lower this distance once Cosmos updates Cyanide.
- Message boxes will appear when you enable/disable everything for testing purposes

V1.02 (May 15, 2014)
- Added infinite ammo
- Statically linked VC2013 Redistributable (... I think)

V1.01 (May 15, 2014)
- Added infinite z-bar
- Added no cooldown on special skills

V1 (May 14, 2014)
- Released
- Vacuum and teleportation
