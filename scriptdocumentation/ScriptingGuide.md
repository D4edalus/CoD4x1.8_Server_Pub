# GSC Scripting Guide

This scripting guide shows how to modify server side scripts on the CoD4x server.

## Quickstart - Running your first script!
In this chapter we develop a small welcome script. It will wait for a player to connect to the server, and write a welcome message to his screen.

### Set up your scriptfile
Inside the serverfolder create a new folder called **main_shared**<sup>[1](#myfootnote1)</sup>. Inside **main_shared** create a file called **welcome.gsc**. This file will contain our code.

```C
init()
{
    for(;;)
    {
        level waittill("connected", player); // Waits until a new player connects - player stored in "player" variable.
        player thread welcome(); // Execute a new thread for "player".
    }
}

welcome()
{
    // "player" is referenced as "self" now.
    self endon("disconnect"); // If player was connected but left without spawning, thread will lock because of next statement.
    self waittill("spawned_player"); // Waits until the player spawned.
    self iprintlnbold("Welcome " + self.name); // Writes the welcome message bold and centered on the player's screen.
}
```

### Execute the script
Just because a script or function is present, doesn't mean it also gets executed. *You not only tell them how to jump, you also tell them when to jump.* 
Do not worry yet about the following lines, what's happening will be explained later.

Create a file called **_callbacksetup.gsc** inside main_shared/maps/mp/gametypes. Fill it with the contents of this file: https://github.com/D4edalus/CoD4MW/blob/master/raw/maps/mp/gametypes/_callbacksetup.gsc. To run our script we are going to edit the existing **CodeCallback_StartGameType** function.

```C
CodeCallback_StartGameType()
{
    // If the gametype has not beed started, run the startup.
    if(!isDefined(level.gametypestarted) || !level.gametypestarted)
    {
          [[level.callbackStartGameType]]();

          level.gametypestarted = true; // So we know that the gametype has been started up.

          thread welcome::init(); // Initializes the welcome script.
    }
}

```

<sub><a name="myfootnote1">1</a>: main_shared is not only one possibility to load custom scripts on the CoD4x server. Most certainly the easiest and the best one for development.</sub>

## Scripting Basics
This tutorial does not cover general programming concepts like Variables, Functions, Arrays, etc. Consider reading [this](http://wiki.modsrepository.com/index.php?title=Call_of_Duty_4:_CoD_Script_Handbook) first if you have no clue about programming at all.

### Language Features

Describes how some not so basic language concepts are used in GSC.

#### Supported Operators
* +, -, *, /
* % modulo
* !, <, >, <=, >=
* <<, >> leftshift and rightshift respectively

#### Supported Types
* int (32 bit signed)
* float (32 bit, IEEE 754)
* string
* array/dictionary
* struct

#### Builtin Functions / Keywords
* for, while, do-while
* break, continue, return
* if, else, switch-case
* wait, waittill, waittillframeend

#### Context
Functions can be run inside a context. The context of a function can be referenced with the **self** keyword from within the function.

```C
SayHelloToPlayer()
{
    self printlnbold("Hello " + self.name);
}

...

player SayHelloToPlayer();
```

#### Function Pointers
Usually you can only define and call functions. However, function pointers let you take a function and store it into a variable for later use. A great example for the use of function pointers is **_callbacksetup.gsc** which was used in the previous chapter. 

Assignment of a function pointer: 
```C
foo() { ... }
bar = ::foo;
```

Assign a function inside another file to a function pointer: 
```C
// foo now is defined in a scriptfile inside a folder main_shared/myscripts/script.gsc
bar = myscripts\script::foo;
```

To call function pointers we use double brackets:
```C
bar = myscripts\script::foo;
[[bar]](); // You can define arguments inside '()' parens if you need to.
```

#### Events
You can emit and wait for events. This enables easy communication between different threads in your code.

Example:
```C
player notify("player_iscamping"); // called when a player was detected camping

...

player thread watchCamping(); // called when the player connects

...

watchCamping()
{
    self endon("disconnect");

    for(;;)
    {
        self waittill("player_iscamping");
        self iprintlnbold("You filthy camper!");
        // do something cruel to the player because he's camping
    }
}
```

Notify with parameter:
```C
notify("player_iscamping", 5.0);
```

Waittill event with parameters:
```C
player waittill("player_iscamping", camptime);
```

Full list of built-in script notifies you can get [here](https://github.com/callofduty4x/CoD4x_Server/blob/master/scriptdocumentation/SCRIPT_NOTIFIES.md).


#### Threads
Threads are used to execute multiple functions in parallel. No worries, you don't have to deal with concurreny and synchronization in GSC. Threads are implemented as Fibers aka resumable functions, and are executed sequentially. 

Starting a new thread:
```C
foo() { ... }
thread foo();
```

End a thread at event
```C
foo()
{
  endon("disconnect");
  
  for(;;) // do something in infinite loop
  {
    ...
  }
}

player thread foo(); // threads gets terminated when player disconnects
```

> Protip: Threads are running until they are paused by wait(seconds) or waittill(event). In case you have many heavy threads you can loadbalance them between serverframes.

> Use `waittillframeend;` to pause the thread and keep executing it on the next serverframe. Rarely needed, better use `wait 0.05;`.


## Advanced Topics
### The Virtual Filesystem

### Serverside Userinterfaces

