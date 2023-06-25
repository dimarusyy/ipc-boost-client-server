$binpath=("{0}\out\install\bin\ipc-boost-server.exe" -f (Get-Location));
Write-Host ('Adding service for : {0}' -f $binpath);

$credentials=new-object -typename System.Management.Automation.PSCredential -argumentlist "NT AUTHORITY\LOCAL SYSTEM";
New-Service -name 'IPC Boost Server' -binaryPathName $binpath -displayName 'IPC Boost Server' -startupType Manual -Credential $credential;
