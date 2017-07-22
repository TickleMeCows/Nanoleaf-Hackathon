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

#define waitBuildUp -1
#define endBuildUp -2
#define buildUpConfirmation 5
#define MAX_SOURCES 10

FrameSlice_t* frameSlices = NULL;
int nFrameSlices = 0;
int transTime = 15;
int hue = 0;

int buildUp;
int prev_beatStrength;
int curr_beatStrength;
int buildUpCounter;

// Our pointer to the saved pointer panel thing.
static RGB_t* paletteColours = NULL;
static LayoutData *layoutData;
static int nColours = 0;

typedef struct {
	float x;
	float y;
	float diffusion_age; // starts from zero and increments for each frame
	int R;
	int G;
	int B;
    float intensity;
    float speed;
} source_t;

static source_t sources[MAX_SOURCES];
static int nSources = 0;

void colourSystem(void);


/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */
void initPlugin() {
	// This is a boolean
	buildUp = 0;
	buildUpCounter = 0;
	// This is the last known beatStrength;
	prev_beatStrength = -1; // Default


	// We rotate the layout because we want to do the horizontal alignment for the slices.

	//grab the layout data, this function returns a pointer to a statically allocated buffer. Safe to call as many time as required.
    //Dont delete this pointer. The memory is managed automatically.
    LayoutData* layoutData = getLayoutData();

    rotateAuroraPanels(layoutData, &layoutData->globalOrientation);

    //quantizes the layout into framelices. See SDK documentation for more information
    getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices, &nFrameSlices, layoutData->globalOrientation);

}


/**
 * A helper function thats fills up the frame array at frameIndex with a specified framelices
 * and a specified hue. the color is the specified hue at 100% saturation and brightness.
 * Note that the FrameSlice_t structure is just a vector of panels at that frame slice
 */
void fillUpFramesArray(FrameSlice_t* frameSlice, Frame_t* frame, int* frameIndex, int hue){
    static RGB_t rgb;
    for (unsigned int i = 0; i < frameSlice->panelIds.size(); i++){
        frame[*frameIndex].panelId = frameSlice->panelIds[i];
        HSVtoRGB((HSV_t){hue, 100, 100}, &rgb);
        frame[*frameIndex].r = rgb.R;
        frame[*frameIndex].g = rgb.G;
        frame[*frameIndex].b = rgb.B;
        frame[*frameIndex].transTime = transTime;
        (*frameIndex)++;
    }
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
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime) {
	// Call our checker each time
	colourSystem();
	int index = 0;
	int spatialHue = hue;
    int hueStep = 15;

	switch(buildUp) {
		case 0:
			// This is the normal behaviour.


			break;
		case 1:
			// This would be the buildUp behaviour.

			//
			for (int i = nFrameSlices/2 -1; i >= 0 ; i--){
				fillUpFramesArray(&frameSlices[i], frames, &index, spatialHue%360);
		        fillUpFramesArray(&frameSlices[nFrameSlices - 1 - i], frames, &index, spatialHue%360);
			}
			*nFrames = index;
			//in a non-music effect, the sleeptime is determined by the plugin itself.
			//Important that this variable is set correctly by the plugin.
			*sleepTime = transTime;


			break;

	}
}



/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup() {
	//do deallocation here
}


/**
	This will be our general algorithm which will decide if there is a beatBuildup
*/
void colourSystem() {

	// How strong of a beatStrength will we allow until we decide the buildUp has ended.
	int drop_offSet;
	// how many times the tempo got faster
	buildUpCounter = 0;
	// Retrieve the current beat data.
	curr_beatStrength = getTempo();

	// Start off case for the system (everytime a buildUp ends it defaults to this case);
	if (prev_beatStrength == waitBuildUp) {

		prev_beatStrength = curr_beatStrength;

	} else {
		// buildUp algorithm has started in the other case.

		// Check if the beat is rising. If it is not we assume buildUp has ended.
		if (prev_beatStrength > curr_beatStrength) {

			// Check if we have increased tempo buildUpConfirmation number of times
			// if we do then this indicates we are infact within a build up portion of the song.
			if (buildUpCounter == buildUpConfirmation) {
				// we Are indeed within a buildup Section of the code.
				buildUp = 1;
				prev_beatStrength = -1;
			}

			buildUpCounter++;
			// buildUp has ended disable it.
		}
	}

}
/**
	Configurations on what happens after the beat has ended.
	Returns 0 on success.
*/
void buildUpEnd() {


	// Need to reset things.
	buildUpCounter = 0;
}
