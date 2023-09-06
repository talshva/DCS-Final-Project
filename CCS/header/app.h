#ifndef _app_H_
#define _app_H_

//------------------------------------------------------------------------------  
//           Global enumerate variables
enum FSMstate{state0,state1,state2,state3,state4, state5, state6, state7, state8, state9};
enum FSM_script{sleep, upload_file1,upload_file2,upload_file3,play_file1, play_file2, play_file3};
enum FSM_telemeter{tele_sleep, tele_action};
enum FSM_object_detector{detector_sleep, detector_action};
enum FSM_light_detector{light_sleep, light_calibrate, light_scan};
enum FSM_light_object_detector{light_object_sleep, light_object_scan};
enum SYSmode{mode0,mode1,mode2,mode3,mode4}; 
//------------------------------------------------------------------------------

#endif



