<#
  Установка разкладокъ клавіатуры безъ MSI — то же, что дѣлаетъ setup.exe изъ MSKLC.

  Архивъ релиза (Kbd-amd64.zip, Kbd-arm64.zip):
    Install-Kbd.ps1, Kbd.cmd
    System32\<Name>.dll   — родная для сѵстемы сборка
    SysWOW64\<Name>.dll   — 32-битная сборка для WOW64 (съ BUILD_WOW6432)

  Запускъ (Kbd.cmd обходитъ ExecutionPolicy для скачаннаго скрипта):
    Kbd                      версіи dll въ архивѣ и въ сѵстемѣ, команды
                             (зелёная версія — новѣе другой, жёлтая — старѣе)
    Kbd -Install KbdSlav     установка (можно нѣсколько: -Install KbdSlav,KbdLat)
    Kbd -Update              замѣнить dll уже установленныхъ разкладокъ (та же версія пропускается,
                             на болѣе старую — только съ согласія)
    Kbd -Uninstall KbdSlav   удаленіе (или изъ «Установленныхъ приложеній»)

  Свѣдѣнія о разкладкѣ берутся изъ самой dll:
    строка 1000 — Layout Text, строка 1200 — языкъ (ru-RU), FileDescription — имя въ спискѣ приложеній.

  Файлы:
    System32\<Name>.dll → %WinDir%\System32\
    SysWOW64\<Name>.dll → %WinDir%\SysWOW64\
    этотъ скриптъ       → %ProgramFiles%\KbdLayouts\Install-Kbd.ps1  (для удаленія)
    Занятую (загруженную) dll скриптъ проситъ отпустить и пробуетъ снова, пока отвѣчаютъ Y/Yes/Д/Да;
    иначе переименовываетъ её въ *.old (удалится при перезагрузкѣ) и пишетъ команду для стиранія.

  Реестръ HKLM:
    SYSTEM\CurrentControlSet\Control\Keyboard Layouts\<KLID>   KLID = a0NN<LCID>, первый свободный
      Layout File          = <Name>.dll
      Layout Text          = строка 1000
      Layout Display Name  = @%SystemRoot%\system32\<Name>.dll,-1000
      Layout Id            = первый свободный съ 00c0
    SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Kbd_<Name>
      DisplayName, DisplayVersion, Publisher, InstallLocation, DisplayIcon,
      UninstallString (этотъ скриптъ съ -Uninstall <Name>), NoModify, NoRepair, EstimatedSize

  Текущій пользователь (безъ -NoLangBar):
    списокъ языковъ (Set-WinUserLanguageList) — разкладка <LCID>:<KLID>
    добавляется къ своему языку, а если такого языка нѣтъ, то и онъ самъ

  При удаленіи всё это стирается.
#>
param(
    [string[]] $Install,
    [string[]] $Uninstall,
    [switch]   $Update,
    [switch]   $NoLangBar
)
$ErrorActionPreference = 'Stop'

# Windows PowerShell пишетъ въ консоль OEM-кодировкою (866), а въ ней нѣтъ ѣ, і, ѳ, ѵ
$oldEnc = [Console]::OutputEncoding
try { [Console]::OutputEncoding = [Text.UTF8Encoding]::new($false) } catch { }

function Say([string] $text, [ConsoleColor] $color = 'Gray', [switch] $NoNewline) {
    Write-Host $text -ForegroundColor $color -NoNewline:$NoNewline
}

# Съ -File запятая списка не дѣлитъ: «-Install KbdSlav,KbdLat» приходитъ одною строкою
$Install   = @($Install   -split ',' | Where-Object { $_ })
$Uninstall = @($Uninstall -split ',' | Where-Object { $_ })

$layouts = 'HKLM:\SYSTEM\CurrentControlSet\Control\Keyboard Layouts'
$uninstRoot = 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall'
$home_   = Join-Path $Env:ProgramFiles 'KbdLayouts'
$srcSys  = Join-Path $PSScriptRoot 'System32'
$srcWow  = Join-Path $PSScriptRoot 'SysWOW64'

# Изъ 32-битнаго процесса System32 перенаправляется — берёмъ Sysnative
$arch = if ($Env:PROCESSOR_ARCHITEW6432) { $Env:PROCESSOR_ARCHITEW6432 } else { $Env:PROCESSOR_ARCHITECTURE }
$sys  = if ($Env:PROCESSOR_ARCHITEW6432) { "$Env:WinDir\Sysnative" } else { "$Env:WinDir\System32" }
$wow  = "$Env:WinDir\SysWOW64"
if ($arch -eq 'x86') { throw '32-битная Windows не поддерживается: въ ней нѣтъ SysWOW64, а 32-битная dll собрана для WOW64' }

Add-Type -Namespace W -Name K -MemberDefinition @'
[DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern bool MoveFileEx(string a,string b,int f);
[DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern IntPtr LoadLibraryEx(string f,IntPtr h,int fl);
[DllImport("kernel32.dll")] public static extern bool FreeLibrary(IntPtr h);
[DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int LoadString(IntPtr h,int id,System.Text.StringBuilder s,int n);
'@

function Get-Res([string] $dll, [int[]] $ids) {
    $h = [W.K]::LoadLibraryEx($dll, [IntPtr]::Zero, 0x22)   # AS_DATAFILE | AS_IMAGE_RESOURCE
    if ($h -eq [IntPtr]::Zero) { throw "Не открыть $dll" }
    try {
        $ids | ForEach-Object { $s = [Text.StringBuilder]::new(256); [void][W.K]::LoadString($h, $_, $s, 256); $s.ToString() }
    } finally { [void][W.K]::FreeLibrary($h) }
}

function Get-Machine([string] $dll) {
    $b = [IO.File]::ReadAllBytes($dll)
    switch ([BitConverter]::ToUInt16($b, [BitConverter]::ToInt32($b, 0x3C) + 4)) {
        0x8664 { 'AMD64' } 0xAA64 { 'ARM64' } 0x014C { 'x86' } default { '?' }
    }
}

# Версія файла или $null, если файла нѣтъ
function Get-Ver([string] $dll) {
    if (-not $dll -or -not (Test-Path $dll)) { return $null }
    $v = (Get-Item $dll).VersionInfo
    [version]::new($v.FileMajorPart, $v.FileMinorPart, $v.FileBuildPart, $v.FilePrivatePart)
}

# Въ сѵстемѣ новѣе, чѣмъ въ архивѣ, — спросить, ставить ли старую
function Confirm-Older([string] $name, [version] $new, [version] $cur) {
    if (-not $cur -or $new -ge $cur) { return $true }
    Say "Въ сѵстемѣ $name $cur — новѣе, чѣмъ въ архивѣ ($new)." Yellow
    $a = Read-Host 'Поставить старую? [y/Да, иначе пропустить]'
    $a.Trim() -match '^(y|yes|д|да)$'
}

function Test-Admin {
    if (([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
            [Security.Principal.WindowsBuiltInRole]::Administrator)) { return $true }
    # На всякій случай — прямо то, что нужно: запись въ HKLM
    try { $k = [Microsoft.Win32.Registry]::LocalMachine.OpenSubKey('SYSTEM\CurrentControlSet\Control\Keyboard Layouts', $true) }
    catch { return $false }
    if ($k) { $k.Close(); return $true }
    $false
}

function Find-Klid([string] $name) {
    Get-ChildItem $layouts | Where-Object { $_.GetValue('Layout File') -and
            [IO.Path]::GetFileNameWithoutExtension($_.GetValue('Layout File')) -eq $name } |
        Select-Object -First 1 -ExpandProperty PSChildName
}

# Фактически установленные файлы — по «Layout File» въ реестрѣ: пути въ System32 и SysWOW64;
# $null — разкладка въ реестрѣ не записана
function Get-SysFiles([string] $name) {
    $klid = Find-Klid $name
    if (-not $klid) { return $null }
    $lf = [Environment]::ExpandEnvironmentVariables((Get-ItemProperty "$layouts\$klid").'Layout File')
    $s  = if ([IO.Path]::IsPathRooted($lf)) { $lf } else { Join-Path $sys $lf }
    , @($s, (Join-Path $wow (Split-Path $lf -Leaf)))
}
# Файлъ занятъ: просимъ отпустить и пробуемъ снова, пока отвѣчаютъ «да»
function Wait-Free([string] $path, [scriptblock] $try) {
    while ($true) {
        try { & $try; return $true } catch { }
        $who = Get-Process | Where-Object { try { $_.Modules.FileName -contains $path } catch { $false } } |
               ForEach-Object { "$($_.ProcessName) ($($_.Id))" }
        Write-Warning "$path занятъ$(if ($who) { ': ' + ($who -join ', ') })"
        $a = Read-Host 'Переключи разкладку на другую (или закрой эти программы). Попробовать снова? [Y/Да]'
        if ($a.Trim() -notmatch '^(y|yes|д|да)$') { return $false }
    }
}

# Не отпустили: прежній файлъ переименовать, удалится при перезагрузкѣ
function Set-Old([string] $path) {
    $old = "$path.$([guid]::NewGuid().ToString('N').Substring(0,8)).old"
    Rename-Item $path (Split-Path $old -Leaf)
    [void][W.K]::MoveFileEx($old, $null, 4)                  # DELAY_UNTIL_REBOOT
    Write-Warning "Прежній файлъ переименованъ и удалится при перезагрузкѣ:"
    Say "  Remove-Item -Force '$old'" Cyan
}

function Put-File([string] $src, [string] $dst) {
    if (Wait-Free $dst { Copy-Item $src $dst -Force -ErrorAction Stop }) { return }
    Set-Old $dst
    Copy-Item $src $dst -Force
}

function Remove-File([string] $path) {
    if (-not (Test-Path $path)) { return }
    if (Wait-Free $path { Remove-Item $path -Force -ErrorAction Stop }) { return }
    Set-Old $path
}

function Copy-Dlls([string] $name) {
    if (-not (Test-Path "$srcWow\$name.dll")) { throw "Нѣтъ SysWOW64\$name.dll" }
    $to = Get-SysFiles $name
    if (-not $to) { $to = "$sys\$name.dll", "$wow\$name.dll" }
    Put-File "$srcSys\$name.dll" $to[0]
    Put-File "$srcWow\$name.dll" $to[1]
}

function Set-LangBar([string] $tag, [string] $tip, [bool] $add) {
    $list = Get-WinUserLanguageList
    if ($add) {
        $lang = $list | Where-Object LanguageTag -eq $tag
        if (-not $lang) { $list.Add($tag); $lang = $list | Where-Object LanguageTag -eq $tag }
        if ($lang.InputMethodTips -notcontains $tip) { $lang.InputMethodTips.Add($tip) }
    } else {
        foreach ($l in $list) { [void]$l.InputMethodTips.Remove($tip) }
    }
    Set-WinUserLanguageList $list -Force
}

function Set-Uninst([string] $name, [string] $dll) {
    $v = (Get-Item $dll).VersionInfo
    $key = "$uninstRoot\Kbd_$name"
    New-Item $key -Force | Out-Null
    $ps = "$Env:WinDir\System32\WindowsPowerShell\v1.0\powershell.exe"
    $vals = @{
        DisplayName     = if ($v.FileDescription) { $v.FileDescription } else { $name }
        DisplayVersion  = Get-Ver $dll
        Publisher       = $v.CompanyName
        InstallLocation = $home_
        DisplayIcon     = "$Env:WinDir\System32\input.dll,0"
        UninstallString = "`"$ps`" -NoProfile -ExecutionPolicy Bypass -File `"$home_\Install-Kbd.ps1`" -Uninstall $name"
    }
    foreach ($k in $vals.Keys) { if ($vals[$k]) { Set-ItemProperty $key $k $vals[$k] } }
    foreach ($k in 'NoModify', 'NoRepair') { New-ItemProperty $key $k -PropertyType DWord -Value 1 -Force | Out-Null }
    New-ItemProperty $key EstimatedSize -PropertyType DWord -Value 20 -Force | Out-Null
}

# Итогъ работы: имя → старая и новая версія System32 ($null — файла нѣтъ)
$results = [ordered]@{}
function Add-Result([string] $name, [version] $old, [version] $new) { $script:results[$name] = $old, $new }

function Do-Install([string] $name) {
    $src = "$srcSys\$name.dll"
    if (-not (Test-Path $src)) { throw "Нѣтъ System32\$name.dll рядомъ со скриптомъ" }
    $m = Get-Machine $src
    if ($m -ne $arch) { throw "$name.dll собрана для $m, а сѵстема ${arch}: нуженъ другой архивъ" }
    $text, $tag = Get-Res $src 1000, 1200
    if (-not $text) { $text = $name }
    $lcid = '{0:x4}' -f ([Globalization.CultureInfo]::GetCultureInfo($(if ($tag) { $tag } else { 'ru-RU' }))).LCID

    $was = if ($cur = Get-SysFiles $name) { Get-Ver $cur[0] }
    if (-not (Confirm-Older $name (Get-Ver $src) $was)) { Say "Пропущена $name" DarkGray; Add-Result $name $was $was; return }
    Copy-Dlls $name

    $klid = Find-Klid $name
    if (-not $klid) {
        $klid = 0..0xFF | ForEach-Object { 'a0{0:x2}{1}' -f $_, $lcid } |
                Where-Object { -not (Test-Path "$layouts\$_") } | Select-Object -First 1
    }
    $key = "$layouts\$klid"
    if (-not (Test-Path $key)) {
        $used = Get-ChildItem $layouts | ForEach-Object { $_.GetValue('Layout Id') } | Where-Object { $_ } |
                ForEach-Object { [Convert]::ToInt32($_, 16) }
        $id = 0xC0; while ($used -contains $id) { $id++ }
        New-Item $key | Out-Null
        New-ItemProperty $key 'Layout Id' -Value ('{0:x4}' -f $id) | Out-Null
    }
    Set-ItemProperty $key 'Layout File' "$name.dll"
    Set-ItemProperty $key 'Layout Text' $text
    New-ItemProperty $key 'Layout Display Name' -PropertyType ExpandString -Value "@%SystemRoot%\system32\$name.dll,-1000" -Force | Out-Null

    New-Item $home_ -ItemType Directory -Force | Out-Null
    if ($PSCommandPath -ne "$home_\Install-Kbd.ps1") { Copy-Item $PSCommandPath "$home_\Install-Kbd.ps1" -Force }
    Set-Uninst $name $src

    if (-not $NoLangBar) { Set-LangBar $tag "$($lcid.ToUpper()):$($klid.ToUpper())" $true }
    Say "Установлена $name ($klid, $(Get-Ver $src))" Green
    Add-Result $name $was (Get-Ver $src)
}

function Do-Update {
    $n = 0
    foreach ($f in Get-ChildItem $srcSys -Filter *.dll -ErrorAction SilentlyContinue) {
        $name = $f.BaseName
        if (-not (Find-Klid $name)) { continue }
        $n++
        $m = Get-Machine $f.FullName
        if ($m -ne $arch) { throw "$name.dll собрана для $m, а сѵстема ${arch}: нуженъ другой архивъ" }
        $new = Get-Ver $f.FullName
        $cur = Get-SysFiles $name
        $was = Get-Ver $cur[0]
        if ($new -eq $was -and $new -eq (Get-Ver $cur[1])) { Say "${name}: $new — та же версія, пропущена" DarkGray; Add-Result $name $was $was; continue }
        if (-not (Confirm-Older $name $new $was)) { Say "Пропущена $name" DarkGray; Add-Result $name $was $was; continue }
        Copy-Dlls $name
        if (Test-Path "$uninstRoot\Kbd_$name") { Set-Uninst $name $f.FullName }
        Say "Обновлена $name" Green
        Add-Result $name $was $new
    }
    if ($n) {
        New-Item $home_ -ItemType Directory -Force | Out-Null
        Copy-Item $PSCommandPath "$home_\Install-Kbd.ps1" -Force
    } else { Say 'Нечего обновлять: ни одна изъ разкладокъ архива не установлена.' Yellow }
}

function Do-Uninstall([string] $name) {
    $cur = Get-SysFiles $name                                    # до удаленія ключа реестра
    if (-not $cur) { $cur = "$sys\$name.dll", "$wow\$name.dll" }
    $was = Get-Ver $cur[0]
    $klid = Find-Klid $name
    if ($klid) {
        Set-LangBar '' "$($klid.Substring(4).ToUpper()):$($klid.ToUpper())" $false
        Remove-Item "$layouts\$klid" -Recurse -Force
    }
    Remove-File $cur[0]
    Remove-File $cur[1]
    Remove-Item "$uninstRoot\Kbd_$name" -Recurse -Force -ErrorAction SilentlyContinue
    Say "Удалена $name" Green
    Add-Result $name $was $null
}

# Версія для таблицы: зелёная — новѣе сравниваемой, жёлтая — старѣе
function Show-Ver([version] $v, [version] $other, [int] $width = 12) {
    if (-not $v) { Say (('{0,-' + $width + '}') -f '—') DarkGray -NoNewline; return }
    $c = if (-not $other -or $v -eq $other) { 'White' } elseif ($v -gt $other) { 'Green' } else { 'Yellow' }
    Say (('{0,-' + $width + '}') -f $v) $c -NoNewline
}

# Установленныя нестандартныя разкладки: KLID съ «a» (такъ ставятъ и этотъ скриптъ, и MSKLC)
function Get-Installed {
    Get-ChildItem $layouts | Where-Object { $_.PSChildName -like 'a*' -and $_.GetValue('Layout File') } |
        ForEach-Object { [IO.Path]::GetFileNameWithoutExtension($_.GetValue('Layout File')) }
}

function Show-State {
    $avail = @(Get-ChildItem $srcSys -Filter *.dll -ErrorAction SilentlyContinue | ForEach-Object BaseName)
    $inst  = @(@(Get-Installed) + @(Get-ChildItem $uninstRoot | Where-Object PSChildName -like 'Kbd_*' |
               ForEach-Object { $_.PSChildName.Substring(4) }) | Sort-Object -Unique)
    $names = @($avail + $inst | Sort-Object -Unique)
    Say 'Разкладки' Cyan
    if (-not $names) { Say '  ни въ архивѣ, ни въ сѵстемѣ' DarkGray }
    if (-not (Test-Path $srcSys)) {
        Say "  Рядомъ со скриптомъ нѣтъ папки System32 — это не распакованный архивъ релиза, ставить не изъ чего." Yellow
    }
    foreach ($n in $names) {
        $klid = Find-Klid $n
        $cur  = Get-SysFiles $n
        Say "  $n" White -NoNewline
        if ($klid) { Say "  $((Get-ItemProperty "$layouts\$klid").'Layout Text') ($klid)" Gray }
        else {
            Say '  въ реестрѣ разкладокъ нѣтъ' DarkGray
            if (Test-Path "$sys\$n.dll") { Say "    файлъ $sys\$n.dll лежитъ, но разкладкою не записанъ" Yellow }
            $cur = $null, $null
        }
        $aSys = Get-Ver "$srcSys\$n.dll"; $aWow = Get-Ver "$srcWow\$n.dll"
        $cSys = Get-Ver $cur[0];          $cWow = Get-Ver $cur[1]
        foreach ($r in @(@('System32', $aSys, $cur[0], $cSys), @('SysWOW64', $aWow, $cur[1], $cWow))) {
            Say ('    {0,-9} архивъ ' -f $r[0]) DarkGray -NoNewline; Show-Ver $r[1] $r[3]
            Say ' сѵстема ' DarkGray -NoNewline; Show-Ver $r[3] $r[1] 0
            if ($r[3]) { Say "  $($r[2])" DarkGray } else { Say '' }
        }
    }
    Say ''
    Say 'Команды' Cyan
    $any = { param($list) if ($list) { $list -join ',' } else { '<имя>' } }
    foreach ($c in @(
        @('Kbd', ''),
        @("Kbd -Install   $(& $any $avail) [-NoLangBar]", 'не добавлять разкладку въ списокъ языковъ -NoLangBar'),
        @('Kbd -Update', 'замѣнить изъ архива'),
        @("Kbd -Uninstall $(& $any $inst)", ''))) {
        Say ('  {0,-43}' -f $c[0]) White -NoNewline; Say $c[1]
    }
    Say ''
    Say 'Вмѣсто Kbd можно: powershell -ExecutionPolicy Bypass -File Install-Kbd.ps1 ...' DarkGray
    Say "Права администратора: $(if (Test-Admin) { 'есть' } else { 'нѣтъ — для установки откроется окно съ ними' })" DarkGray
}

try {
    # Безъ параметровъ — что въ архивѣ и что въ сѵстемѣ
    if (-not ($Install -or $Uninstall -or $Update)) { Show-State; return }

    # Безъ правъ администратора — то же въ новомъ окнѣ съ ними (повысить права въ этомъ окнѣ Windows не даётъ)
    if (-not (Test-Admin)) {
        Say 'Нужны права администратора: открываю новое окно съ ними.' Yellow
        $a = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-NoExit', '-File', "`"$PSCommandPath`"")
        if ($Install)   { $a += '-Install',   ($Install -join ',') }
        if ($Uninstall) { $a += '-Uninstall', ($Uninstall -join ',') }
        if ($Update)    { $a += '-Update' }
        if ($NoLangBar) { $a += '-NoLangBar' }
        Start-Process (Get-Process -Id $PID).Path -ArgumentList $a -Verb RunAs
        return
    }

    foreach ($n in $Install)   { Do-Install $n }
    if ($Update)               { Do-Update }
    foreach ($n in $Uninstall) { Do-Uninstall $n }

    # Итогъ: «старая → новая»; не тронутая — одна версія
    if ($results.Count) {
        Say ''; Say 'Итогъ' Cyan
        foreach ($n in $results.Keys) {
            $o, $v = $results[$n]
            $os = if ($o) { "$o" } else { '—' }; $vs = if ($v) { "$v" } else { 'удалена' }
            Say ('  {0,-10} ' -f $n) White -NoNewline
            if ($o -eq $v) { Say $os Gray } else { Say "$os → $vs" Green }
        }
    }

    # Удалили послѣднюю — убрать и копію скрипта (она можетъ сейчасъ исполняться)
    if ($Uninstall -and -not (Get-ChildItem $uninstRoot | Where-Object PSChildName -like 'Kbd_*')) {
        Start-Process cmd.exe -ArgumentList "/c timeout 2 >nul & rmdir /s /q `"$home_`"" -WindowStyle Hidden
    }
} catch {
    Say $_.Exception.Message Red
    exit 1
} finally {
    try { [Console]::OutputEncoding = $oldEnc } catch { }
}
