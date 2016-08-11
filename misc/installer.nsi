; Much of this was copied from Battle for Wesnoth

!ifdef INNER
  SetCompress off                           ; for speed
!else
  SetCompressor /SOLID lzma
!endif

!define Company "Nooskewl"
!define Version "2.12.2"
!define Game "Monster RPG 2"
!define ExeName "MonsterRPG2"

!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "Software\${Company}\${Game}\${Version}"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME ""
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "Software\${Company}\${Game}\${Version}"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME ""
!define MULTIUSER_INSTALLMODE_INSTDIR "${Company}\${Game} ${Version}"

!include 'LogicLib.nsh'
!include "MultiUser.nsh"
!include "MUI2.nsh"
!include 'sections.nsh'

;--------------------------------
;General
 
  Name "${Game}"
  BrandingText "© 2016 ${Company}"

!ifdef INNER
  ;!echo "Inner invocation"                  ; just to see what's going on
  OutFile "$%TEMP%\tempinstaller.exe"       ; not really important where this is
!else
  ;!echo "Outer invocation"
 
  ; Call makensis again, defining INNER.  This writes an installer for us which, when
  ; it is invoked, will just write the uninstaller to some location, and then exit.
  ; Be sure to substitute the name of this script here.
 
  !system "$\"${NSISDIR}\makensis$\" /DINNER C:\Users\Trent\code\monster-rpg-2\installer.nsi" = 0
 
  ; So now run that installer we just created as %TEMP%\tempinstaller.exe.  Since it
  ; calls quit the return value isn't zero.
 
  !system "$%TEMP%\tempinstaller.exe" = 2
 
  ; That will have written an uninstaller binary for us.  Now we sign it with your
  ; favourite code signing tool.
 
  !system "SIGNTOOL.EXE SIGN /f C:\Users\Trent\Documents\codesign-2016-2017.pfx /p MYPASSWORDHERE /t http://timestamp.comodoca.com/authenticode $%TEMP%\Uninstall.exe" = 0
 
  ; Good.  Now we can carry on writing the real installer.
 
  OutFile "${ExeName}-${Version}-Windows.exe"
!endif

;--------------------------------
;Variables
 
  Var StartMenuFolder

;--------------------------------
;Interface Settings
 
  !define MUI_WELCOMEFINISHPAGE_BITMAP "icons\win32_installer_logo.bmp"
  !define MUI_HEADERIMAGE_BITMAP "icons\installer_head_logo.bmp"
  !define MUI_ICON "icons\windows_icon.ico"
  !define MUI_UNICON "icons\windows_icon.ico"
  !define MUI_ABORTWARNING
 
;--------------------------------
;Language Selection Dialog Settings
 
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "SHCTX" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${Company}\${Game}\${Version}" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
 
;--------------------------------
;Pages
 
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "docs\FamilyLicense.txt"
  !insertmacro MULTIUSER_PAGE_INSTALLMODE
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
 
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "${Company}\${Game}"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "SHCTX" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${Company}\${Game}\${Version}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
 
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
 
  !insertmacro MUI_PAGE_INSTFILES
 
  !define MUI_FINISHPAGE_RUN "$INSTDIR\${ExeName}.exe"
  !insertmacro MUI_PAGE_FINISH
 
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
 
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "PortugueseBR"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Spanish"
 
;--------------------------------
;Reserve Files
 
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
 
  !insertmacro MUI_RESERVEFILE_LANGDLL
  ReserveFile "${NSISDIR}\Plugins\*.dll"
 
;--------------------------------
;Installer Sections
 
Section ; Main section
 
  SetOutPath "$INSTDIR"
  File /r /x .* build\full\*.*
 
  ;Create uninstaller
 !ifndef INNER
  ; this packages the signed uninstaller
 
  File $%TEMP%\Uninstall.exe
!endif

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
 
  ;Create shortcuts
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${Game}.lnk" "$INSTDIR\${ExeName}.exe"
 
  !insertmacro MUI_STARTMENU_WRITE_END

  ;DirectX redist
  SetOutPath "$INSTDIR\REDIST"
  File /r /x .* misc\REDIST\DIRECTX

  ;For Programs and Features
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "Contact" "contact@nooskewl.ca"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "DisplayIcon" "$INSTDIR\${ExeName}.exe,0"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "DisplayName" "${Game} ${Version}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "DisplayVersion" "${Version}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "HelpLink" "http://nooskewl.ca"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "Publisher" "${Company}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\" /$MultiUser.InstallMode"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /$MultiUser.InstallMode /S"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "URLInfoAbout" "http://nooskewl.ca/contact"
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "EstimatedSize" 30000
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "NoModify" 1
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}" "NoRepair" 1

  ; Install DirectX redist if not installed
  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\DirectX" "SDKVersion"
  StrCpy $2 $1 2
  StrCmp "$2" "9." installed
  ExecWait '$INSTDIR\REDIST\DIRECTX\DXSETUP.exe /silent'
  installed:

  SetOutPath "$INSTDIR"

  ; Remove the redists
  RMDir /r "$INSTDIR\REDIST"

SectionEnd

Section /o "Desktop Shortcut" DesktopShortcutSection

  CreateShortCut "$DESKTOP\${Game}.lnk" "$INSTDIR\${ExeName}.exe"

  WriteRegDWORD SHCTX "Software\${Company}\${Game}\${Version}" "UninstallDesktopShortcut" 1

SectionEnd

;--------------------------------
;Installer Functions
 
Function .onInit
!ifdef INNER
 
  ; If INNER is defined, then we aren't supposed to do anything except write out
  ; the installer.  This is better than processing a command line option as it means
  ; this entire code path is not present in the final (real) installer.
 
  WriteUninstaller "$%TEMP%\Uninstall.exe"
  Quit  ; just bail out quickly when running the "inner" installer
!endif 

  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Descriptions
 
  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC
  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${DesktopShortcutSection} "Create a shortcut on all users' or current user's desktop."
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

!ifdef INNER

Section "Uninstall"

  Delete "$INSTDIR\bass.dll"
  Delete "$INSTDIR\data.zip"
  Delete "$INSTDIR\FamilyLicense.txt"
  Delete "$INSTDIR\${ExeName}.exe"
  Delete "$INSTDIR\ReadMe.txt"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"
  RMDir "$INSTDIR\.." ; Remove company directory if empty

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
 
  Delete "$SMPROGRAMS\$StartMenuFolder\${Game}.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  ReadRegDWORD $1 SHCTX "Software\${Company}\${Game}\${Version}" "UninstallDesktopShortcut"

  ${If} $1 == 1
    Delete "$DESKTOP\${Game}.lnk"
  ${EndIf}

  DeleteRegKey SHCTX "Software\${Company}\${Game}\${Version}"
  DeleteRegKey /ifempty SHCTX "Software\${Company}\${Game}"
  DeleteRegKey /ifempty SHCTX "Software\${Company}"
  DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${Game} ${Version}"
 
SectionEnd

!endif

;--------------------------------
;Uninstaller Functions
 
Function un.onInit
 
  !insertmacro MULTIUSER_UNINIT
  !insertmacro MUI_UNGETLANGUAGE
 
FunctionEnd
