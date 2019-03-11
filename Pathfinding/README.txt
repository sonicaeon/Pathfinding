/////////////////////////Installation/////////////////////////
By default Pathfinding comes with two grids: Grid.grid and 
NoPathGrid.grid. These are located in ../Pathfinding/Pathfinding/Content

If you wish to use your own Grid you will have to copy your grid
to the above directory. Also, to select which grid to use:
Open up PathfindingGame.cpp and navigate to Line: 32 and replace the following
"Content\\Grid.grid"; with:
The syntax for hardcoding your grid would be: "Content\\<yourgridnamehere>.grid";
replacing <yourgridnamehere> with the name of the file of your grid.
(Sorry! UI for selecting a grid from file explorer is currently a work in progress)

Launch Pathfinding.sln located in ../Pathfinding/Pathfinding.sln
Restore/download required NuGet packages (Bespoke.games.framework.1.4.3)
and their dependencies.

Build the Solution/Project, it should be able to build any
of the configuration modes (DEBUG/Release @ WIN32/x64).

////////////Controls/Instructions (Keyboard/Mouse)////////////
* ESC - Quit the game
* Select Start Node - Left Click on a Grid Space
* Select End Node - Left Click on a Grid Space (always after a Start Node Selection)

Select any combination of Algorithm and Heuristic you would like to use
(However, please note that, Heuristics only apply to Greedy Best-First and
A* Algorithms and will not change the results for Breadth First Search and
Dijkstra's)

The Find Path button will use the current settings in the menu and then
compute the path (if one exists) and also display the number of nodes visited, 
the number of nodes it would take from start to get to the end node, and the
time (ms) to compute the search.

The Reset button can be used if things somehow get out of sync (shouldn't occur)
or if you just wish to return to the default settings when you launched the .exe
(Note: the default starting places for Start and End are (0,0) and (9,9) respectively.)