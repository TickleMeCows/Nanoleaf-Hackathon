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



int buildUp;
int prev_beatStrength;
int curr_beatStrength;

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
	// This is the last known beatStrength;
	prev_beatStrength = -1; // Default
	curr_beatStrength;

	//grab the layout data, this function returns a pointer to a statically allocated buffer. Safe to call as many time as required.
    //Dont delete this pointer. The memory is managed automatically.
    LayoutData* layoutData = getLayoutData();

    rotateAuroraPanels(layoutData, &layoutData->globalOrientation);

    //quantizes the layout into framelices. See SDK documentation for more information
    getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices, &nFrameSlices, layoutData->globalOrientation);

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


			break;

	}
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
	int buildUpCounter = 0;
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


/**
	This function will be responsible for recieving the beatData.

	Return: Beat Strength.
	Returns: -1 on error.
*/
int beatData() {
	int beatStrength;


	return beatStrength;
}

// This is responsible for obtaining all the lights and putting them into a list.

/** Removes a light source from the list of light sources */
void removeSource(int idx)
{
	memmove(sources + idx, sources + idx + 1, sizeof(source_t) * (nSources - idx - 1));
	nSources--;
}

/** Compute cartesian distance between two points */
float distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/**
 * @description: Get a colour by interpolating in a linear way amongs the set of colours in the palette
 * @param colour The colour we want between 0 and nColours - 1. We interpolate in the palette to come up
 *               with a final colour somewhere in the palettes spectrum.
 */
void getRGB(float colour, int *returnR, int *returnG, int *returnB)
{
    float R;
    float G;
    float B;

	if(nColours == 0) {
	    *returnR = 128; // in the case of no palette, use half white as default
	    *returnG = 128;
	    *returnB = 128;
	}
	else if(nColours == 1) {
	    *returnR = paletteColours[0].R;
	    *returnG = paletteColours[0].G;
	    *returnB = paletteColours[0].B;
	}
	else {
	    int idx = (int)colour;
	    float fraction = colour - (float)idx;

        if(colour <= 0) {
            R = paletteColours[0].R;
            G = paletteColours[0].G;
            B = paletteColours[0].B;
        }
        else if(idx < nColours - 1) {
            float R1 = paletteColours[idx].R;
            float G1 = paletteColours[idx].G;
            float B1 = paletteColours[idx].B;
            float R2 = paletteColours[idx + 1].R;
            float G2 = paletteColours[idx + 1].G;
            float B2 = paletteColours[idx + 1].B;
            R = (1.0 - fraction) * R1 + fraction * R2;
            G = (1.0 - fraction) * G1 + fraction * G2;
            B = (1.0 - fraction) * B1 + fraction * B2;
        }
        else {
            R = paletteColours[nColours - 1].R;
            G = paletteColours[nColours - 1].G;
            B = paletteColours[nColours - 1].B;
        }
	    *returnR = (int)R;
	    *returnG = (int)G;
	    *returnB = (int)B;
	}
}

/**
  * @description: Adds a light source to the list of light sources. The light source will have a particular
  * colour and intensity and be centred on a randomly chosen panel
*/
void addSource(float colour, float intensity, float speed)
{
    int r = (int)(drand48() * layoutData->nPanels);
    float x = layoutData->panels[r].shape->getCentroid().x;
    float y = layoutData->panels[r].shape->getCentroid().y;

    // decide in the colour of this light source and factor in the intensity to arrive at an RGB value
	int R;
	int G;
	int B;
    getRGB(colour, &R, &G, &B);
    R *= intensity;
    G *= intensity;
    B *= intensity;

    // if we have too many light sources then remove the oldest one
    if(nSources >= MAX_SOURCES) {
    	removeSource(0);
    }

    int i;
    for(i = nSources; i > 0; i--) {
        if(intensity >= sources[i - 1].intensity) {
            break;
        }
   }

    memmove(sources + i + 1, sources + i, sizeof(source_t) * (nSources - i));

    // save the information in the list
    sources[i].x = x;
    sources[i].y = y;
    sources[i].diffusion_age = 0.0;
    sources[i].R = R;
    sources[i].G = G;
    sources[i].B = B;
    sources[i].intensity = intensity;
    sources[i].speed = speed;
    nSources++;
}
