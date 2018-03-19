Set-PSDebug -Trace 1
Set-ExecutionPolicy RemoteSigned

# Configure Conda to operate without user input
conda config --set always_yes yes --set changeps1 no
conda config --add channels conda-forge
conda config --add channels wheeler-microfluidics
conda config --add channels sci-bots
conda config --add channels dropbot

# Allow extra Conda channels to be added (e.g., for testing).
if ($env:CONDA_EXTRA_CHANNELS) {
    $env:CONDA_EXTRA_CHANNELS.Split(";") | ForEach {
        Write-Host "Adding Conda channel from %CONDA_EXTRA_CHANNELS%: $_"
        conda config --add channels $_
    }
}


# Use PSCondaEnvs to allow activation using powershell:
conda install -n root -c pscondaenvs pscondaenvs

# Update conda, and install conda-build (used for building in non-root env)
conda update -q conda
conda install --yes conda-build anaconda-client nose

# Create new project environment
conda create --name $env:APPVEYOR_PROJECT_NAME python anaconda-client

# Use patch (from `conda-build`) to support Appveyor Python 2.7 win-64 build.
if ($env:ARCH -eq "x64") {
  conda install --yes curl 7za
  # Download patch to support Appveyor Python 2.7 win-64 build.
  cmd /C curl -L -o conda-build-3.7.1.zip https://github.com/conda/conda-build/archive/3.7.1.zip
  # Extract patch.
  7za x .\conda-build-3.7.1.zip conda-build-3.7.1\ci\appveyor
  # Apply patch.
  & conda-build-3.7.1\ci\appveyor\setup_x64.bat
  copy "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\vcvars64.bat" "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\amd64\vcvarsamd64.bat"
}
