EEGLogger
=====

## Goal

+ Construct a dynamic error cancelation filter, using computer vision
  - Useful for correcting EEG signal errors
  - Should be used in conjuction with other filters
    * High pass
    * Low pass
    * 60 Hz removal

+ Remove signals due to music infiltration in the EEG signal
  - Requires classification of music
  - Then removal of associated music interferance

## Experimental Method

+ Record a video/eeg of a user as they have various states, over 5 minutes
  - 0   -  20 seconds: "Blank Face"
  - 21  -  40 seconds: Blinking
  - 41  -  60 seconds: Right Wink
  - 61  -  80 seconds: Left Wink
  - 81  - 100 seconds: Fake Talk
  - 101 - 120 seconds: Furrow Brow
  - 121 - 140 seconds: Smile
  - 141 - 160 seconds: Squint
  - 161 - 180 seconds: Scratch Right Ear
  - 181 - 200 seconds: Scratch Left Ear
  - 201 - 220 seconds: Scratch Right Face
  - 221 - 240 seconds: Scratch Left Face
  - 241 - 260 seconds: Close Eyes& Count to 15
  - 261 - 280 seconds: Act Like Chewing Gum
  - 281 - 300 seconds: View Flashing Box @ 5 Hz

+ Track face and normalize
  - Accomplished by tracking the face
  - Constructing a retangle around the face
  - Construct a grid inside the rectangle
  - Determine movement inside each grid selection (CV algorithm)

+ Compare EEG signals with grid movement
  - Associating each EEG signal with a given grid movement
  - Accomplished by using feature recognition
  - Specifically, search for alpha, theta, beta waves

