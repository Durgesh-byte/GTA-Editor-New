param([string]$server="",[Int32]$port=0 ) 
 $client = New-Object Net.Sockets.TcpClient; 
 try 
 { 
  $client.Connect($server, $port);  
  Write-Output $true 
 } 
 catch 
 { 
   Write-Output $false 
 } 
 finally 
  { 
   $client.Dispose() 
 }