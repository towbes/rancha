# rancha
 Multibox Character Launcher for Dark Age of Camelot

## Usage Instructions

- Rancha expects your DAOC installation to be at C:\Program Files (x86)\Electronic Arts\Dark Age of Camelot\

- Download the repository and unzip to a directory.  Rancha.exe and login.dll have to be in the same directory.

- The directory should be writeable so rancha can save character profiles into a folder called 'accounts'

- When you first launch rancha you will be at the Login screen.  There are two ways to log in:

![Login Screen](https://github.com/towbes/rancha/blob/main/images/imguiLogin.JPG?raw=true)

1. Account Name + Password + Server

2. Account Name + Password + Character Name + Realm + Server

- If you click "Login" DAOC will launch and log in with the input you supplied.

- If you click "Save", rancha will save that information to a file "<accountName>" in the current working directory in a folder called 'accounts'
# rancha
 Multibox Character Launcher for Dark Age of Camelot

## Usage Instructions

- Rancha expects your DAOC installation to be at C:\Program Files (x86)\Electronic Arts\Dark Age of Camelot\

- Download the repository and unzip to a directory.  Rancha.exe and login.dll have to be in the same directory.

- The directory should be writeable so rancha can save character profiles into a folder called 'accounts'

- When you first launch rancha you will be at the Login screen.  There are two ways to log in:

![Login Screen](https://github.com/towbes/rancha/blob/main/images/imguiLogin.JPG?raw=true)

1. Account Name + Password + Server

2. Account Name + Password + Character Name + Realm + Server

- If you click "Login" DAOC will launch and log in with the input you supplied.

- If you click "Save Account", rancha will save that information to a file "<accountName>" in the current working directory in a folder called 'accounts'

- If you click "Save Charcter", rancha will save that character information to a file "<accountName>" in the current working directory in a folder called 'accounts'

- You can then click File -> Accounts to view the saved accounts screen:

![Profiles](https://github.com/towbes/rancha/blob/main/images/imguiaccts.JPG?raw=true)

- Click on the account you want to log in and select a server, then press the Login button.

- You can then click File -> Characters to view the saved characters screen:

![Profiles](https://github.com/towbes/rancha/blob/main/images/imguiChars.JPG?raw=true)

- Click on the character you want to log in, then press the Login button.

## What it does

- Saves characters for each account name in separate files
 
- You can save multiple characters for each account.  Enter the character names one at a time and click save (while keeping the same account information entered)

- Allows you to launch more than 2 DAOC windows (by stopping the DAOC mutex)

- Uses ImGui for UI, and cereal for saving/loading files
 
 ## Known Issues / To Do items
 
 - Would like to add the "teams" feature to launch multiple characters at once

- You can then click File -> Saved Profiles to view the profiles screen:

![Profiles](https://github.com/towbes/rancha/blob/main/images/Profiles.JPG?raw=true)

- Click on the character you want to log in, then press the Login button.

## What it does

- Saves characters for each account name in separate files
 
- You can add save multiple characters for each account.  Enter the character names one at a time and click save (while keeping the same account information entered)

- Allows you to launch more than 2 DAOC windows (by stopping the DAOC mutex)
 
 ## Known Issues / To Do items
 
 - At this time you have to save specific character information.  If you just save account + password it will not show on the listview
 
 - Would like to add the "teams" feature to launch multiple characters at once
