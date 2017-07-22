/*
    Copyright 2017 Nanoleaf Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */



#include "AuroraPlugin.h"
#include "LayoutProcessingUtils.h"
#include "ColorUtils.h"
#include "DataManager.h"
#include "PluginFeatures.h"
#include "Logger.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initPlugin();
	void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime);
	void pluginCleanup();

#ifdef __cplusplus
}
#endif

/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */
void initPlugin(){

}

/**
 * @description: this the 'main' function that gives a frame to the Aurora to display onto the panels
 * To obtain updated values of enabled features, simply call get<feature_name>, e.g.,
 * getEnergy(), getIsBeat().
 *
 * If the plugin is a sound visualization plugin, the sleepTime variable will be NULL and is not required to be
 * filled in
 * This function, if is an effects plugin, can specify the interval it is to be called at through the sleepTime variable
 * if its a sound visualization plugin, this function is called at an interval of 50ms or more.
 *
 * @param frames: a pre-allocated buffer of the Frame_t structure to fill up with RGB values to show on panels.
 * Maximum size of this buffer is equal to the number of panels
 * @param nFrames: fill with the number of frames in frames
 * @param sleepTime: specify interval after which this function is called again, NULL if sound visualization plugin
 */
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime){

}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup(){
	//do deallocation here
}


/**
	This will be our general algorithm which will
*/
int colourSystem(){
	// This is a boolean
	int buildUp;
	// This is the last known beatStrength;
	int prev_beatStrength = -1; // Default
	int curr_beatStrength;

	// Retrieve the current beat data.
	if ((curr_beatStrength = beatData()) == -1){
		printf("Couldnt recieve the current beat data\n");
	}

	// Start off case for the system (everytime a buildUp ends it defaults to this case);
	if (prev_beatStrength == -1){
		prev_beatStrength = curr_beatStrength;
	} else {
		// buildUp algorithm has started in the other case.

		// Check if the beat is rising. If it is not we assume buildUp has ended.
		if (prev_beatStrength > current_beatStrength) {

		}
	}

	// How strong of a beatStrength will we allow until we decide the buildUp has ended.

	int drop_offSet;


	return 0;
}
/**
	Configurations on what happens after the beat has ended.
	Returns 0 on success.
*/
int buildUpEnd(){


}


/**
	This function will be responsible for recieving the beatData.

	Return: Beat Strength.
	Returns: -1 on error.
*/
int beatData(){
	int beatStrength;


	return beatStrength;
}
