# Flixerch
This app currently in active development it is free to use and distributed under the MIT lisence.

No releases yet. Our target date for release version 1.0.0 is late October 2026.

Current status. 

We manage to create a basic renderer for quads and images, we do need to go back later to make those more robust and preformance in mind.

Following immidiate tasks are:
- Finishing our UI logic to work with the renderer, and create a basic home page.
- Finishing the job system and start working with it.
- Finishing the http request parser and start using data we get from online resources.

  
## Buiding
For building on windows environment.
### Option A - The fastest
Clone the repo 
```batch
    git clone https://github.com/Ultra-Filter/flixerch
```
Enter the repo directory
```batch
    cd flixerch
```
Run the builder script (Debug mode)
```batch
    .\build_system\builder.cmd d
```
Run the builder script (Release mode)
```batch
    .\build_system\builder.cmd r
```
Run the executable
Debug: 
```batch
    .\build\debug\main.exe
```
Release:
```batch
    .\build\release\main.exe
```
### Option B - The robust
Go to file `build_system\build.c` change the name of main function from `_main` to `main`.
Run 
```batch
    gcc -o bld.exe build_system\build.c
```
Now you can run:
For building Debug version 
```batch
    .\bld.exe +bd
```
For Release version 
```batch
    .\bld.exe +br
```
For cleaning cache you can add the `+c` flag, and for information about the build you can add the `+i` flag.

Example 
Running with clean cache and information on release build.
```batch
    .\bld.exe +br +c +i
```

## Guidelines

### Project structure

- src
    - app.c
    - backend
        - recommendation_system
        - search_engine
        - database
        - web 
    - core
        - job_system
        - video_decoder
        - audio_decoder
    - frontend
        - renderer
        - ui_system



## TODO's

### Week 1
- Basic renderer fubctionality ![Static Badge](https://img.shields.io/badge/in_progress-blue)
- Basic job system functionality ![Static Badge](https://img.shields.io/badge/no_progress-red)
- Basic web functionality  ![Static Badge](https://img.shields.io/badge/no_progress-red)

**GOAL** *By the end of the week we want an executable that launches a window and display an image from the web loaded using curl on different thread.* ![Static Badge](https://img.shields.io/badge/success-green)

### week 2 
- Render functionality improvment (multi-size text rendering, ui-logic, demo home page)
- Web functionality execute http request and parse responds (Basic)
- Job system functionality

**GOAL** *by the end of this week we want to see a demo page with scrollable content which displayes triangles with images it loaded at runtime from TMDB*
