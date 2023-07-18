## Installing Inno Setup

```text
cd installers
rm -rf InnoSetup
WINEDEBUG=-all wine ./innosetup-6.2.2.exe /verysilent /dir=InnoSetup /noicons /tasks= /portable=1
rm -rf InnoSetup/Examples
```

