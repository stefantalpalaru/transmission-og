#define AppName 'Transmission OG'
#define AppVersion GetVersionNumbersString('..\tmp_install\bin\transmission-og-gtk.exe')
#define AppGUID "{CD87E0B3-C1C1-4B23-BD95-DFD5F7A7691B}"

[Setup]
AppId={{#AppGUID}
AppName={#AppName}
AppVerName={#AppName} {#AppVersion}
ArchitecturesInstallIn64BitMode=x64
Compression=lzma2
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
LZMANumBlockThreads=16
LZMAUseSeparateProcess=yes
OutputBaseFilename={#AppName} Installer-x64-{#AppVersion}
OutputDir=installer
PrivilegesRequiredOverridesAllowed=dialog
SolidCompression=yes
UninstallDisplayIcon={app}\transmission-og-gtk.exe
VersionInfoVersion={#AppVersion}
WizardStyle=modern

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; For importing DLL functions at setup.
Source: "UninsIS\UninsIS.dll"; Flags: dontcopy
; Our installed files.
Source: "..\tmp_install\bin\*"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\tmp_install\share\transmission-og\web\*"; DestDir: "{app}\web"; Flags: ignoreversion recursesubdirs
Source: "..\tmp_install\share\icons\*"; DestDir: "{app}\share\icons"; Flags: ignoreversion recursesubdirs
Source: "..\tmp_install\share\pixmaps\*"; DestDir: "{app}\share\pixmaps"; Flags: ignoreversion recursesubdirs
Source: "..\tmp_install\share\glib-2.0\*"; DestDir: "{app}\share\glib-2.0"; Flags: ignoreversion recursesubdirs
Source: "..\tmp_install\README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Menu entry.
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\transmission-og-gtk.exe"
; Desktop icon.
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\transmission-og-gtk.exe"; Tasks: desktopicon

[Code]
// Automatically uninstall an existing installation. From https://github.com/Bill-Stewart/UninsIS

// Import IsISPackageInstalled() function from UninsIS.dll at setup time
function DLLIsISPackageInstalled(AppId: string; Is64BitInstallMode,
  IsAdminInstallMode: DWORD): DWORD;
  external 'IsISPackageInstalled@files:UninsIS.dll stdcall setuponly';

// Import CompareISPackageVersion() function from UninsIS.dll at setup time
function DLLCompareISPackageVersion(AppId, InstallingVersion: string;
  Is64BitInstallMode, IsAdminInstallMode: DWORD): LongInt;
  external 'CompareISPackageVersion@files:UninsIS.dll stdcall setuponly';

// Import UninstallISPackage() function from UninsIS.dll at setup time
function DLLUninstallISPackage(AppId: string; Is64BitInstallMode,
  IsAdminInstallMode: DWORD): DWORD;
  external 'UninstallISPackage@files:UninsIS.dll stdcall setuponly';

// Wrapper for UninsIS.dll IsISPackageInstalled() function
// Returns true if package is detected as installed, or false otherwise
function IsISPackageInstalled(): Boolean;
begin
  result := DLLIsISPackageInstalled('{#AppGUID}',  // AppId
    DWORD(Is64BitInstallMode()),                   // Is64BitInstallMode
    DWORD(IsAdminInstallMode())) = 1;              // IsAdminInstallMode
  if result then
    Log('UninsIS.dll - Package detected as installed')
  else
    Log('UninsIS.dll - Package not detected as installed');
end;

// Wrapper for UninsIS.dll CompareISPackageVersion() function
// Returns:
// < 0 if version we are installing is < installed version
// 0   if version we are installing is = installed version
// > 0 if version we are installing is > installed version
function CompareISPackageVersion(): LongInt;
begin
  result := DLLCompareISPackageVersion('{#AppGUID}',  // AppId
    '{#AppVersion}',                                  // InstallingVersion
    DWORD(Is64BitInstallMode()),                      // Is64BitInstallMode
    DWORD(IsAdminInstallMode()));                     // IsAdminInstallMode
  if result < 0 then
    Log('UninsIS.dll - This version {#AppVersion} older than installed version')
  else if result = 0 then
    Log('UninsIS.dll - This version {#AppVersion} same as installed version')
  else
    Log('UninsIS.dll - This version {#AppVersion} newer than installed version');
end;

// Wrapper for UninsIS.dll UninstallISPackage() function
// Returns 0 for success, non-zero for failure
function UninstallISPackage(): DWORD;
begin
  result := DLLUninstallISPackage('{#AppGUID}',  // AppId
    DWORD(Is64BitInstallMode()),                 // Is64BitInstallMode
    DWORD(IsAdminInstallMode()));                // IsAdminInstallMode
  if result = 0 then
    Log('UninsIS.dll - Installed package uninstall completed successfully')
  else
    Log('UninsIS.dll - installed package uninstall did not complete successfully');
end;


function PrepareToInstall(var NeedsRestart: Boolean): string;
begin
  result := '';
  // If package installed, uninstall it automatically if the version we are
  // installing does not match the installed version; If you want to
  // automatically uninstall only...
  // ...when downgrading: change <> to <
  // ...when upgrading:   change <> to >
  //if IsISPackageInstalled() and (CompareISPackageVersion() <> 0) then
  if IsISPackageInstalled() then
    UninstallISPackage();
end;
