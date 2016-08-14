param (
	[Parameter(Mandatory=$true)][string]$sourcePath,
	[Parameter(Mandatory=$true)][string]$outPath,
	[Parameter(Mandatory=$true)][string]$compilerPath
)

foreach ($item in Get-ChildItem $sourcePath) 
{
	$rawShaderPath = [IO.Path]::GetFullPath($sourcePath + $item);
	$outputShaderPath = [IO.Path]::GetFullPath($outPath + $item);
	Write-Host $rawShaderPath;
	Write-Host $outputShaderPath;
	& $compilerPath $rawShaderPath -V -o $outputShaderPath;
}
