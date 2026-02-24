# Define the replacements
$replacements = @{
    'ToolsVersion="15.0"' = 'ToolsVersion="4.0"'
    '<WindowsTargetPlatformVersion>10.0.17763.0</WindowsTargetPlatformVersion>' = '<WindowsTargetPlatformVersion>8.1</WindowsTargetPlatformVersion>'
    '<PlatformToolset>v141</PlatformToolset>' = '<PlatformToolset>v100</PlatformToolset>'
}

# Get all .vcxproj files - Note: Ensure you are in the root directory
$files = Get-ChildItem -Filter *.vcxproj -Recurse

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    
    # Perform each replacement
    foreach ($old in $replacements.Keys) {
        $content = $content -replace [regex]::Escape($old), $replacements[$old]
    }
    
    # FIXED: Changed 'UTF-8' to 'UTF8' to satisfy the enumerator requirement
    Set-Content -Path $file.FullName -Value $content -Encoding UTF8
    Write-Host "Updated: $($file.Name)" -ForegroundColor Green
}
