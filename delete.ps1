$namespace = "root\subscription"

$filterName = "BootEventFilter"
$consumerName = "BootPowerShellConsumer"

# Get objects
$filter = Get-WmiObject -Namespace $namespace -Class __EventFilter -Filter "Name='$filterName'"
$consumer = Get-WmiObject -Namespace $namespace -Class CommandLineEventConsumer -Filter "Name='$consumerName'"

# Remove binding
if ($filter -and $consumer) {
    Get-WmiObject -Namespace $namespace -Class __FilterToConsumerBinding |
        Where-Object {
            $_.Filter -like "*$filterName*" -and $_.Consumer -like "*$consumerName*"
        } | ForEach-Object {
            $_.Delete()
        }
}

# Remove consumer
if ($consumer) {
    $consumer.Delete()
}

# Remove filter
if ($filter) {
    $filter.Delete()
}

Write-Host "WMI subscription cleanup completed."