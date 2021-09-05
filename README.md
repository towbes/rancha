# rancha
 Multibox Character Launcher for Dark Age of Camelot

## Usage Instructions

- Rancha expects your DAOC installation to be at C:\Program Files (x86)\Electronic Arts\Dark Age of Camelot\

- Download the repository and unzip to a directory.  Rancha.exe and login.dll have to be in the same directory.

- The directory should be writeable so rancha can save character profiles into a folder called 'accounts'

- When you first launch rancha you will be at the Login screen.  There are two ways to log in:

![Login Screen](https://github.com/towbes/rancha/blob/main/images/LoginScreen.JPG?raw=true)

1. Account Name + Password + Server

2. Account Name + Password + Character Name + Realm + Server

- If you click "Login" DAOC will launch and log in with the input you supplied.

- If you click "Save", rancha will save that information to a file "<accountName>" in the current working directory in a folder called 'accounts'

- You can then click File -> Saved Profiles to view the profiles screen:

![Profiles](https://github.com/towbes/rancha/blob/main/images/Profiles.JPG?raw=true)

- Click on the character you want to log in, then press the Login button.

## What it does

- Saves characters for each account name in separate files

- Allows you to launch more than 2 DAOC windows (by stopping the DAOC mutex)