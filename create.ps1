$namespace = "root\subscription"

$filterName = "BootEventFilter"
$consumerName = "BootPowerShellConsumer"

# 1. Event Filter 
$filterArgs = @{
    Name = $filterName
    EventNamespace = "root\cimv2"
    QueryLanguage = "WQL"
    Query = "SELECT * FROM __InstanceModificationEvent WITHIN 60 
             WHERE TargetInstance ISA 'Win32_PerfFormattedData_PerfOS_System' 
             AND TargetInstance.SystemUpTime < 240"
}

$filter = Set-WmiInstance -Namespace $namespace -Class __EventFilter -Arguments $filterArgs

# 2. Consumer 
$consumerArgs = @{
    Name = $consumerName
    CommandLineTemplate = "powershell.exe -ExecutionPolicy Bypass -NoProfile -Command `"Write-Output 'Boot Triggered' >> C:\bootlog.txt`""
}

$consumer = Set-WmiInstance -Namespace $namespace -Class CommandLineEventConsumer -Arguments $consumerArgs

# 3. Bind
$bindingArgs = @{
    Filter = $filter
    Consumer = $consumer
}

Set-WmiInstance -Namespace $namespace -Class __FilterToConsumerBinding -Arguments $bindingArgs