# Coding_Two_Advanced_Frameworks_FinalWork

Introduction
===========
Inspired by my previous research abou Human-Computing Integration, I tried to explore the relationship between the integrated body and the self-image in this project by openframeworks. 

This project represents a process by which my computer understands my actions. On the left is the live image my computer is seeing, which is showing how I’m trying to show my computer what I’m doing. The colourful circle mean the computer is capturing my actions, and there is some fuzz that indicates a bug in the computer’s capturing. One the right is how my computer is understanding my situation internally. It can't completely restore my appearance, it seems to be always covered by a black curtain.

In summary,  I want to propose a question through this project, what kind of impact does a machine that cannot understand and cooperate with humans have on a person's self-image? As a part of my research, I will continue related theoretical research and continue my computer graphics practice.


OpenFrameworks
==============
In this project, it mainly consists of three parts:
Colour circles.
Blur.
Dark gray interface.

Colour circles are made by ofxCv, an openframeword addon. By calculating the RGB value of the corresponding pixel in the real-time image and the difference between adjacent pixels, the outline of the object is found, and circles and rectangles are drawn. Blur is made through shaders. By calling the configured motionAmplifier.h file, the pixels whose RGB changes exceed a certain threshold and their surrounding pixels are enlarged and blurred. The dark grey interface uses the ofxCv plug-in to make the webcam record the previous image and the current image, merge them, and then average the RGB values of each vertical line to achieve this effect.
