Symbols used in the document:
****xxxx => xxxx is a Running Instruction.
////xxxx => xxxx is a Personal Comment.

////The Code is present in mainfile.cpp

Requirements:
OS : Linux (Ubuntu)
OpenCV installed (minimum required version 2.4)
cmake is installed.

How to Run:
Type the following on terminal:
cmake .
make
./mainfile

If cmake is not installed, type the following to compile while including the OpenCV libraries:
g++ mainfile.cpp -o mainfile2 -lopencv_core -lopencv_imgproc -lopencv_highgui
./mainfile2
////Not Sure if this method would work properly on machine other than the one in which it is made

After running, a Terminal window opens and asks for the type of effect to run.
Select the Approprite Number.
////Pressing inappropriate keys may terminate the program.
A window showing the effect opens up.
****To quit from the effect, make sure your ACTIVE SCREEN is the WINDOW SHOWING IMAGE (and NOT terminal) and PRESS q.
****In case of Snow Effect, press s to make more snow fall again. Again, make sure your ACTIVE SCREEN is the WINDOW SHOWING IMAGE (and NOT terminal).
////SnowFall is done again automatically after 5 seconds too for the obstructed snow.
////The program contains the code to automatically cleanup the Terminal Window after each effect is done.


Approach:
For taking in video feed, Image is taken constantly in a while loop.
Every time an imamge is taken, it is flipped so that video looks natural (as if we are seeing in a mirror).

1. Ghost Effect: 
For Ghost Effect, a vector is made (of size 30 here).
30 consecutive images are stored in these 30 images.
Thus, these vector would help in accessing a previous Image (the basic requirement for Ghost effect).
Let the index of current image be i. Index of a previous image is (i+(30/2))%30. We take the weighted average of the respective BGR intensities of the two images and show it.
The weight can be modified through a trackbar available to the user. The weight values range from 0 to 10. By default it is set to 5.
So, by changing weight values, The proportion of old image changes. Here, the weight value is weight of current image, (10-weight) that of old image.
The Intensity formula is given by (w is the weight): Intensity = (CurrentImage*w + OldImage*(10-w))*10.
If w=0, Only old image is shown. Therefore, all the transitions will be delayed.
If w=10, No Ghost effect is there as only current image is shown.

2. Old Movie Effect:
The image is converted into Sepia.
Different kinds of distrubances are put in the image.
Some spots of radius 2 are put. The number of these spots are random and varies from 0 to 6 (both inclusive).
50 small spots (1 pixel) are put at random points throughout the image.
A rectangle is displayed in the image. It is shown at an interval of 150 loops.

3. Snow Effect:
A vector for snow points is made.
It has the following structure. 
i) Point (x & y coordinates). 
ii) Speed (an int to denote speed of falling, varies from 1 to 10 pixels per loop). 
iii) Time (If a snow particle is obstructed by an object, it resets all its values after 5 seconds.)
First, the image is converted to a Canny Image (stored in Canny_Out).
Then, we find contours on that image using findContours function.
For each contour and each snow particle, if they contour obstructs the image, that snow particle stops falling).
An obstructed snow particle starts falling again after an interval of 5 seconds.
If we want the snow to fall again manually, Press s.
When coded this way, it feels like all the snow particles fall and reset at the same time.
A different approach to avoid this problem was used earlier, but it had several other implemntation problems and didn't work properly.
The main problem was that the number of contours was very Large and was not constant. So, instead of time as a factor for reset, a counter was used. But as the for loop has dirrent and very large number of repetetins, it could not be implemented properly.

