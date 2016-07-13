
#ifndef __CONST_H
#define __CONST_H

//----------------------------------------------------------------------------------------
/**
 * \file       Consts.h
 * \author     Petr Beránek
 * \date       2015/05/09
 * \brief      File containing constants used in application
 *
 *  This file contains constants used in application.
 *
*/
//----------------------------------------------------------------------------------------

namespace beranpe6
{

///basic parameters of window
#define WINDOW_WIDTH   1300
#define WINDOW_HEIGHT  750
#define WINDOW_TITLE  "Cloud Factory"

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

///parameters describing speed of camera rotations
#define ROT_X_PARAM	 8.0f
#define ROT_Y_PARAM	 8.0f
#define ROT_Z_PARAM	 8.0f

///parameters describing speed of camera movement
#define CAMERA_SPEED 0.07f

///size of test object
#define TEST_OBJECT_SIZE 1.0f



///paths used for loading models and textures
#define TEST_OBJECT  "data/models/testWrench1.obj"


#define SKYBOX_PREFIX  "data/textures/sky"

#define MODEL_TREE_TRUNK  "data/models/tree/TreeTrunk.obj"
#define MODEL_TREE_CROWN  "data/models/tree/TreeCrown.obj"

#define MODEL_HOUSE_BASE  "data/models/house/HouseBase.obj"
#define MODEL_ROOF_HOLDER  "data/models/house/RoofHolder.obj"
#define MODEL_ROOF_TOP  "data/models/house/RoofTop.obj"

#define MODEL_WINDOW_ROUND  "data/models/house/windows/WindowRound.obj"
#define MODEL_GLASS_ROUND  "data/models/house/windows/GlassRound.obj"
#define MODEL_FRAME_TOP  "data/models/house/windows/FrameTop.obj"
#define MODEL_FRAME_BOTTOM  "data/models/house/windows/FrameBottom.obj"
#define MODEL_FRAME_SIDES  "data/models/house/windows/FrameSides.obj"
#define MODEL_GLASS_SQUARE  "data/models/house/windows/GlassSquare.obj"

#define MODEL_DOOR_BODY  "data/models/house/doors/DoorBody.obj"
#define MODEL_DOOR_FRAME  "data/models/house/doors/DoorFrame.obj"
#define MODEL_DOOR_KNOB  "data/models/house/doors/DoorKnob.obj"

#define MODEL_TRANSFORMER_SPIRE  "data/models/house/energyBox/TransformerSpire.obj"
#define MODEL_TRANSFORMER_COILS  "data/models/house/energyBox/TransformerCoils.obj"
#define MODEL_TRANSFORMER_ROOF_HALF  "data/models/house/energyBox/TransformerRoofHalf.obj"
#define MODEL_TRANSFORMER_BODY  "data/models/house/energyBox/TransformerBody.obj"
#define MODEL_TRANSFORMER_FRAME  "data/models/house/energyBox/TransformerFrame.obj"
#define MODEL_TRANSFORMER_HOLDER  "data/models/house/energyBox/TransformerHolder.obj"

#define MODEL_FACTORY_CONNECTOR_BODY  "data/models/house/factory/FactoryConnectorBody.obj"
#define MODEL_FACTORY_CONNECTOR_FRAME  "data/models/house/factory/FactoryConnectorFrame.obj"
#define MODEL_FACTORY_COVERS  "data/models/house/factory/FactoryCovers.obj"
#define MODEL_FACTORY_SHED_PLATE  "data/models/house/factory/FactoryShedPlate.obj"
#define MODEL_FACTORY_SHED_RIVETS  "data/models/house/factory/FactoryShedRivets.obj"
#define MODEL_FACTORY_SHED_ROOF  "data/models/house/factory/FactoryShedRoof.obj"
#define MODEL_FACTORY_SHED_WALLS  "data/models/house/factory/FactoryShedWalls.obj"
#define MODEL_FACTORY_TANK_BODY  "data/models/house/factory/FactoryTankBody.obj"
#define MODEL_FACTORY_TANK_HOLDER  "data/models/house/factory/FactoryTankHolder.obj"
#define MODEL_FACTORY_TANK_HOOP  "data/models/house/factory/FactoryTankHoop.obj"
#define MODEL_FACTORY_TUBINGS  "data/models/house/factory/FactoryTubings.obj"

#define MODEL_PIECES_EXHAUST  "data/models/house/pieces/Exhaust.obj"
#define MODEL_PIECES_EXHAUST_INSIDE  "data/models/house/pieces/ExhaustInside.obj"
#define MODEL_PIECES_LADDER_HOLDER  "data/models/house/pieces/LadderHolder.obj"
#define MODEL_PIECES_LADDER_LONGS  "data/models/house/pieces/LadderLongs.obj"
#define MODEL_PIECES_LADDER_SHORTS  "data/models/house/pieces/LadderShorts.obj"
#define MODEL_PIECES_PLANK  "data/models/house/pieces/Plank.obj"
#define MODEL_PIECES_PLANK_NAILS  "data/models/house/pieces/PlankNails.obj"

#define MODEL_ISLAND_MAIN_BASE  "data/models/island/IslandMainBase.obj"
#define MODEL_ISLAND_MAIN_GRASS  "data/models/island/IslandMainGrass.obj"

#define MODEL_ISLAND_GEARS_AXIS  "data/models/island/gears/GearAxis.obj"
#define MODEL_ISLAND_GEARS_BIG_HOOP  "data/models/island/gears/GearBigHoop.obj"
#define MODEL_ISLAND_GEARS_BIG_INSIDE  "data/models/island/gears/GearBigInside.obj"
#define MODEL_ISLAND_GEARS_SMALL  "data/models/island/gears/GearSmall.obj"

#define MODEL_ISLAND_CLOUD_TANK  "data/models/island/cloudMaker/CloudTank.obj"
#define MODEL_ISLAND_CLOUD_TANK_EXHAUST  "data/models/island/cloudMaker/CloudTankExhaust.obj"
#define MODEL_ISLAND_CLOUD_TANK_METALS  "data/models/island/cloudMaker/CloudTankMetals.obj"

#define MODEL_AIRSHIP_ANCHOR  "data/models/airship/AirshipAnchor.obj"
#define MODEL_AIRSHIP_BALLOON  "data/models/airship/AirshipBalloon.obj"
#define MODEL_AIRSHIP_BALLOON_BELT  "data/models/airship/AirshipBalloonBelt.obj"
#define MODEL_AIRSHIP_BALLOON_BELT_RIVETS  "data/models/airship/AirshipBalloonBeltRivets.obj"
#define MODEL_AIRSHIP_BALLOON_HOLDER_AXIS  "data/models/airship/AirshipBalloonHolderAxis.obj"
#define MODEL_AIRSHIP_BALLOON_HOLDER_BOXES  "data/models/airship/AirshipBalloonHolderBoxes.obj"
#define MODEL_AIRSHIP_BALLOON_HOLDER_ROPES  "data/models/airship/AirshipBalloonHolderRopes.obj"
#define MODEL_AIRSHIP_BATH  "data/models/airship/AirshipBath.obj"
#define MODEL_AIRSHIP_BOXES  "data/models/airship/AirshipBoxes.obj"
#define MODEL_AIRSHIP_FAN_BASE  "data/models/airship/FanBase.obj"
#define MODEL_AIRSHIP_FAN_SAIL_RAILS  "data/models/airship/FanSailRails.obj"
#define MODEL_AIRSHIP_FAN_SAILS  "data/models/airship/FanSails.obj"

#define MODEL_CLOUD_A  "data/models/clouds/CloudA.obj"
#define MODEL_CLOUD_B  "data/models/clouds/CloudB.obj"
#define MODEL_CLOUD_C  "data/models/clouds/CloudC.obj"
#define MODEL_CLOUD_D  "data/models/clouds/CloudD.obj"
#define MODEL_CLOUD_E  "data/models/clouds/CloudE.obj"

#define MODEL_BUTTERFLY_BOLT  "data/models/butterfly/ButterflyBolt.obj"
#define MODEL_BUTTERFLY_NUT_L  "data/models/butterfly/ButterflyNutL.obj"
#define MODEL_BUTTERFLY_NUT_S  "data/models/butterfly/ButterflyNutS.obj"
#define MODEL_BUTTERFLY_WING_L  "data/models/butterfly/ButterflyWingL.obj"
#define MODEL_BUTTERFLY_WING_S  "data/models/butterfly/ButterflyWingS.obj"
#define LANTERN_MAT "data/models/lantern/LanternMat.txt"

#define CLOUDS_TEXTURE_NAME "data/textures/clouds.png"
#define STAR_TEXTURE_NAME "data/textures/star_strip.png"

///size of bounding shapes blocking camera from going out of them
#define BOUNDING_BOX 20.0f;
#define BOUNDING_SPHERE 23.0f;

}

#endif