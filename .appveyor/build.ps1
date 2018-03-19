Set-PSDebug -Trace 1
Set-ExecutionPolicy RemoteSigned

# Set version number based on git tag
$x = git describe --tags
if (!$?){ $prevTag="v0.0-0"
} else  { $prevTag = $x.Split("-")[0] + "-" + $x.Split("-")[1] }

$buildTag = $prevTag + "+" + $(Get-Date -Format FileDateTime)
Write-Host "Build Tag: $buildTag"
Update-AppveyorBuild -Version $buildTag

activate.ps1 $env:APPVEYOR_PROJECT_NAME
$build_status = "Success"

# Set environment variable for project directory (may be used in bld.bat)
$env:project_directory = (Get-Item -Path ".\" -Verbose).FullName
Write-Host "Project directory: $($env:project_directory)"

if ($env:PYTHON_VERSION -eq "2.7") {
  if ($env:ARCH -eq "32") { & "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86 }
  if ($env:ARCH -eq "64") { & "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64 }
}

# Build Package
conda build .conda-recipe --dirty --croot bld
$exit_code = $?
echo $exit_code
if (!$exit_code) { $build_status = "Failed Conda Build Stage" }
$src_dir = $(ls bld *$($env:APPVEYOR_PROJECT_NAME)* -Directory)[0].FullName
if (!$src_dir) {
  $msg = "Cannot find src_dir: the project name may not match conda name."
  $details = "This will cause nosetests to fail."
  Add-AppveyorMessage -Message $msg -Category Error -Details $details
}

Write-Host "SRC Directory: $($src_dir)"

# Delete working environment
conda build purge

# Build package again without skipping tests
$packages_built = $(dir bld\*\*.tar.bz2) -ne $()
if (!$packages_built) { $build_status = "Failed to build Conda packages" }
Write-Host "Build status: $build_status"

# Capture package location and build status
touch BUILD_STATUS
touch PACKAGE_LOCATION
echo $build_status > BUILD_STATUS
# Set exit code to 1 if package(s) weren't built.
if (!$packages_built) { exit 1 }

# Check for broken packages.
Get-ChildItem bld\*\*.tar.bz2 | `
  Select-Object Name, @{ n = 'Folder'; e = { Convert-Path $_.PSParentPath } },
    @{ n = 'Foldername'; e = { ($_.PSPath -split '[\\]')[-2] } },
    FullName | ForEach {
      if ($_.Foldername -eq "broken") {
        exit 1
      }
    }

# Prepend platform (i.e., `win-32`, `win-64`, `noarch`) to each package filename
# and collect package files in `artifacts` directory.
md artifacts
Get-ChildItem bld\*\*.tar.bz2 | `
  Select-Object Name, @{ n = 'Folder'; e = { Convert-Path $_.PSParentPath } },
    @{ n = 'Foldername'; e = { ($_.PSPath -split '[\\]')[-2] } },
    FullName | ForEach {
      echo "$($_.FullName.Trim()) -> artifacts\$($_.Foldername)-$($_.Name)"
      # Upload to `nadamq` Anaconda Cloud channel (only non-broken packages).
      anaconda -t $env:anaconda_token upload -u nadamq $_.FullName.Trim()
      mv $($_.FullName.Trim()) artifacts\$($_.Foldername)-$($_.Name)
    }
