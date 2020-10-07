# RefitKF_Cursor

Implementation of the Re-FIT Kalman Filter for EEG Cursor Control
-Re-FIT Kalman Filter original paper (https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3638087/.


## Dependencies
- Recording Device: Compumedics 64-channel Quik-Cap (1000Hz)
- Amplifier: Compumedics Synamps 2 headbox
- Processing Software: Compumedics Curry 7
- Software Environment:
- Matlab (decoder): Signal Processing Toolbox
- C++ (cursor application): SDL 2.0.4 (https://libsdl.org/download-2.0.php)

## Instructions
1. Connect the EEG cap to your computer.
2. Open Curry 7 and make sure it detects the EEG cap. 
3. Run bci_init.m to initilize the pipeline (raw signal -> Matlab decoder -> C++ Cursor Application)
4. Compile and run Center_out.cpp to start the cursor task.
5. Start recording. 

For Re-FIT Kalman filter online retraining: 
Pause the EEG recording and run "Retrain.m", then resume recording. The decoder parameters will be updated. 
