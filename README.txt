Boilerplate code provided by Troy Alderson (see the Modelling-OtherPrograms repository for modifications).

CONTROLS
ESC: 		Close the OpenGL Window
SPACE: 		Hide the geometric visualization of the algorithm (green lines) and the User's 'u' parameter (red X)
SHIFT:		Hide the position of control points and knots(white and red points). Editing the curve is not recommended
UP/DOWN: 	Increase/decrease the order of the curve
LEFT/RIGHT: 	Move the 'u' parameter (red X) along the curve
ENTER: 		Print statistics about the curve (order, # of points, current value of 'u' parameter)
LEFT CLICK: 	When the cursor is on a point, hold to move it around
		When it is off a point, create a new end point for the curve
RIGHT CLICK: 	Delete the point the cursor is on
MIDDLE CLICK:	Insert a point at the index of the point the cursor is on

COLLABORATORS
None among students, but big thanks to Troy for helping me figure out the geometric visualization of the algorithm

COMPILE INSTRUCTIONS
Type 'make && ./myprogram' on terminal for the current directory.
