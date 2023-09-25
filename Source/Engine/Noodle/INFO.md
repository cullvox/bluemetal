# Config

We created our own configuration language called NWDL (pronounced noodle).
It stands for No Whitespace Data Language. NWDL provides functionality that many
other configuration languages do not allow.


## Benefits
* Simpler than JSON
* Looks like TOML
* Whitespace Doesn't Matter
* Consequentially Unrestricted Style
* Ease of use
* Grouping
* Comments

## Why create *another* language?
Other languages are also simple, easy to use, and work without spaces. NWDL 
doesn't really need to exist, JSON does what it does as of right now with more features and more people understand it. However this language is slightly 
simpler and requires a little less to get up and running. My current 
implementation isn't even that great either, but it does works and is simple 
enough for most people to understand. The implementation will get better with 
time, it's already had a rework.

## Examples
### Simple Game Config
```TOML
# Information about the window
window = {
    width = 1920,
    height = 1080,
    fullscreen = 1,
    monitor = 0
}
render = {
    physicalDevice = 1
}
```

### A Different Style
```TOML
# Using TOML syntax highlighting is kinda weird with this example
window = 
{
    size = { width = 1920, height = 1080 },
    fullscreen = 1, monitor = 0
}
render = 
{
    api = "Vulkan", 
    physicalDevice = 1,
    settings =
    {
        presentMode = 1,
    }
}
```
